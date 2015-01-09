#include "php.h"
#include "lib/script_cfi_utils.h"
#include "lib/script_cfi_hashtable.h"
#include "cfg.h"
#include "cfg_handler.h"
#include "dataset.h"
#include "metadata_handler.h"
#include "compile_context.h"

#define ROUTINE_NAME_LENGTH 256

typedef struct _compilation_unit_t {
  const char *path;
  uint hash;
} compilation_unit_t;

typedef struct _compilation_routine_t {
  const char *name;
  uint hash;
  control_flow_metadata_t cfm;
} compilation_routine_t;

typedef struct _function_fqn_t {
  compilation_unit_t unit;
  compilation_routine_t function;
} function_fqn_t;

static sctable_t routine_table;

void init_compile_context()
{
  routine_table.hash_bits = 7;
  sctable_init(&routine_table);
}

void function_compiled()
{
  uint i, eval_id;
  bool is_script_body = false, is_eval = false;
  function_fqn_t *fqn;
  control_flow_metadata_t cfm;
  uint routine_key;
  const char *function_name;
  char *buffer, *filename, routine_name[ROUTINE_NAME_LENGTH];

  if (CG(active_op_array)->type == ZEND_EVAL_CODE) { // lambda or plain eval?
    is_eval = true;
    eval_id = get_next_eval_id();
    sprintf(routine_name, "<eval-%d>", eval_id);
  } else if (CG(active_op_array)->type == ZEND_USER_FUNCTION) {
    filename = CG(active_op_array)->filename->val;
    
    if (CG(active_op_array)->function_name == NULL) {
      function_name = "<script-body>";
      is_script_body = true;
    } else {
      function_name = CG(active_op_array)->function_name->val;
      if (strcmp(function_name, "__lambda_func") == 0) {
        is_eval = true;
        eval_id = get_next_eval_id();
        sprintf(routine_name, "<default>:lambda_%d", EG(lambda_count)+1);
      }
    }
  } else {
    PRINT("Warning: skipping unrecognized op_array of type %d\n", CG(active_op_array)->type);
    return;
  }
  
  if (is_eval) {
    filename = "<any-file>";
  } else {
    const char *classname;
    if (is_script_body) {
      classname = strrchr(CG(active_op_array)->filename->val, '/') + 1;
    } else {
      if (CG(active_op_array)->scope == NULL)
        classname = "<default>";
      else
        classname = CG(active_op_array)->scope->name->val;
    }
    sprintf(routine_name, "%s:%s", classname, function_name);
  }
  
  routine_key = hash_string(routine_name);
  
  PRINT("--- Function compiled: %s|%s\n", filename, routine_name);
  
  fqn = malloc(sizeof(function_fqn_t));
  buffer = malloc(strlen(filename) + 1);
  strcpy(buffer, filename);
  fqn->unit.path = buffer;
  if (is_eval)
    fqn->unit.hash = EVAL_HASH;
  else
    fqn->unit.hash = hash_string(fqn->unit.path);
  
  buffer = malloc(strlen(routine_name) + 1);
  strcpy(buffer, routine_name);
  fqn->function.name = buffer;
  if (is_eval)
    fqn->function.hash = eval_id;
  else
    fqn->function.hash = hash_string(fqn->function.name);
  
  cfm.dataset = dataset_routine_lookup(fqn->unit.hash, 0);  
  cfm.cfg = routine_cfg_new(fqn->unit.hash, fqn->function.hash);
  fqn->function.cfm.dataset = cfm.dataset;
  fqn->function.cfm.cfg = cfm.cfg;
  
  sctable_add(&routine_table, routine_key, fqn);
  
  for (i = 0; i < CG(active_op_array)->last; i++) {
    uint target;
    zend_op *op = &CG(active_op_array)->opcodes[i];
    if (zend_get_opcode_name(op->opcode) == NULL)
      continue;
    
    routine_cfg_assign_opcode(cfm.cfg, op->opcode, op->extended_value, i);

    switch (op->opcode) {
      case ZEND_JMP:
        target = op->op1.opline_num;
        break;
      case ZEND_JMPZ:
      case ZEND_JMPNZ:
      case ZEND_JMPZNZ:
      case ZEND_JMPZ_EX:
      case ZEND_JMPNZ_EX:
        target = op->op2.opline_num;
        break;
      default:
        continue;
    }
    
    routine_cfg_add_edge(cfm.cfg, i, target);
    PRINT("\t[create edge %d->%d for {%s|%s, 0x%x|0x%x}]\n", i, target,
          fqn->unit.path, fqn->function.name,
          fqn->unit.hash, fqn->function.hash);
  }
  
  if (cfm.cfg->unit_hash == EVAL_HASH) // ??
    dataset_match_eval(&cfm);
  
  if (cfm.dataset == NULL) {
    cfg_opcode_t *cfg_opcode;
    cfg_opcode_edge_t *cfg_edge;
    for (i = 0; i < cfm.cfg->opcodes.size; i++) {
      cfg_opcode = routine_cfg_get_opcode(cfm.cfg, i);
      PRINT("\t[emit %s at %d for {%s|%s, 0x%x|0x%x}]\n", 
            zend_get_opcode_name(cfg_opcode->opcode), i,
          fqn->unit.path, fqn->function.name,
          fqn->unit.hash, fqn->function.hash);
      write_node(fqn->unit.hash, fqn->function.hash, cfg_opcode, i);
    }
    for (i = 0; i < cfm.cfg->edges.size; i++) {
      cfg_edge = routine_cfg_get_edge(cfm.cfg, i);
      write_op_edge(fqn->unit.hash, fqn->function.hash, cfg_edge->from_index, cfg_edge->to_index);
    }
  } else if (cfm.cfg->unit_hash != EVAL_HASH) {
    cfg_opcode_edge_t *cfg_edge;
    for (i = 0; i < cfm.cfg->opcodes.size; i++) {
      dataset_routine_verify_opcode(cfm.dataset, i, 
                                    routine_cfg_get_opcode(cfm.cfg, i)->opcode);
    }
    for (i = 0; i < cfm.cfg->edges.size; i++) {
      cfg_edge = routine_cfg_get_edge(cfm.cfg, i);
      dataset_routine_verify_compiled_edge(cfm.dataset, cfg_edge->from_index, cfg_edge->to_index);
    }
  }
}

const char *get_function_declaration_path(const char *routine_name)
{
  function_fqn_t *fqn;

  fqn = (function_fqn_t *) sctable_lookup(&routine_table, hash_string(routine_name));
  if (fqn == NULL)
    return "unknown";
  else
    return fqn->unit.path;
}

control_flow_metadata_t *get_cfm(const char *routine_name)
{
  function_fqn_t *fqn;

  fqn = (function_fqn_t *) sctable_lookup(&routine_table, hash_string(routine_name));
  if (fqn == NULL)
    return NULL;
  else
    return &fqn->function.cfm;
}
