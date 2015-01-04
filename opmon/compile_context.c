#include "php.h"
#include "lib/script_cfi_utils.h"
#include "lib/script_cfi_hashtable.h"
#include "cfg.h"
#include "cfg_handler.h"
#include "dataset.h"
#include "metadata_handler.h"
#include "compile_context.h"

#define EVAL_ID "|eval|"

typedef struct _compilation_unit_t {
  const char *path;
  uint hash;
} compilation_unit_t;

typedef struct _compilation_routine_t {
  const char *name;
  uint hash;
  uint index;
  control_flow_metadata_t cfm;
} compilation_routine_t;

typedef struct _function_fqn_t {
  compilation_unit_t unit;
  compilation_routine_t function;
} function_fqn_t;

static compilation_unit_t unit_stack[256];
static compilation_unit_t *unit_frame;
static compilation_unit_t *current_unit;
static compilation_unit_t *live_unit;

static compilation_routine_t routine_stack[256];
static compilation_routine_t *routine_frame;
static compilation_routine_t *current_routine;
static compilation_routine_t no_routine = { "<none>", 0 };

static sctable_t routine_table;

void init_compile_context()
{
  unit_frame = unit_stack;
  unit_frame->path = NULL;
  current_unit = NULL;
  
  routine_frame = routine_stack;
  routine_frame->name = NULL;
  routine_frame->cfm.cfg = NULL;
  routine_frame->cfm.dataset = NULL;
  current_routine = &no_routine;
  
  routine_table.hash_bits = 7;
  sctable_init(&routine_table);
}

void push_compilation_unit(const char *path)
{
  ASSERT(path != NULL);
  
  PRINT("> Push compilation unit %s\n", path);
  
  unit_frame->path = path;
  unit_frame->hash = hash_string(path);
  current_unit = unit_frame;
  unit_frame++;
  
  routine_frame->name = "<script-body>";
  routine_frame->hash = 0;
  routine_frame->cfm.dataset = dataset_routine_lookup(current_unit->hash, 0);
  routine_frame->cfm.cfg = routine_cfg_new(current_unit->hash, routine_frame->hash);
  routine_frame->index = 0;
  current_routine = routine_frame;
  routine_frame++;
}

control_flow_metadata_t pop_compilation_unit()
{
  control_flow_metadata_t cfm = current_routine->cfm;
  
  PRINT("> Pop compilation unit\n");
  
  pop_compilation_function();
  
  unit_frame--;
  current_unit = unit_frame - 1;
  
  return cfm;
}

const char *get_compilation_unit_path() 
{
  return current_unit->path;
}

uint get_compilation_unit_hash()
{
  return current_unit->hash;
}

void push_compilation_function(const char *function_name)
{
  function_fqn_t *fqn;
  char *buffer;
  
  if (current_unit->hash == EVAL_HASH)
    return; // handle lambda functions the same as plain evals
  
  // function_name = EVAL_FUNCTION_NAME;
  
  PRINT("> Push compilation function %s\n", function_name);
  
  routine_frame->name = function_name;
  routine_frame->hash = hash_string(function_name);
  routine_frame->cfm.dataset = dataset_routine_lookup(current_unit->hash, routine_frame->hash);
  routine_frame->cfm.cfg = routine_cfg_new(current_unit->hash, routine_frame->hash);
  routine_frame->index = 0;
  current_routine = routine_frame;
  routine_frame++;
  
  fqn = malloc(sizeof(function_fqn_t));
  fqn->unit = *current_unit;
  buffer = malloc(strlen(current_unit->path) + 1);
  strcpy(buffer, current_unit->path);
  fqn->unit.path = buffer;
  fqn->function = *current_routine;
  buffer = malloc(strlen(current_routine->name) + 1);
  strcpy(buffer, current_routine->name);
  fqn->function.name = buffer;
  sctable_add(&routine_table, current_routine->hash, fqn);
}

void pop_compilation_function()
{
  uint i;
  
  PRINT("> Pop compilation function\n");
  
  if (current_routine->cfm.cfg->unit_hash == EVAL_HASH)
    dataset_match_eval(&current_routine->cfm);
  
  if (current_routine->cfm.dataset == NULL) {
    for (i = 0; i < current_routine->cfm.cfg->opcode_count; i++) {
      PRINT("[emit %s at %d for {%s|%s, 0x%x|0x%x}]\n", 
            zend_get_opcode_name(current_routine->cfm.cfg->opcodes[i].opcode), i,
            get_compilation_unit_path(), get_compilation_routine_name(),
            get_compilation_unit_hash(), get_compilation_routine_hash());
      write_node(current_unit->hash, current_routine->hash, 
                 &current_routine->cfm.cfg->opcodes[i], i);
    }
  } else if (current_routine->cfm.cfg->unit_hash != EVAL_HASH) {
    for (i = 0; i < current_routine->cfm.cfg->opcode_count; i++) {
      dataset_routine_verify_opcode(current_routine->cfm.dataset, i, 
                                    current_routine->cfm.cfg->opcodes[i].opcode);
    }
  }
  
  routine_frame--;
  current_routine = routine_frame - 1;
}

const char *get_compilation_routine_name() 
{
  return current_routine->name;
}

uint get_compilation_routine_hash()
{
  return current_routine->hash;
}

void push_eval(uint eval_id)
{
  PRINT("> Push eval 0x%x\n", eval_id);
  
  unit_frame->path = EVAL_PATH;
  unit_frame->hash = EVAL_HASH;
  current_unit = unit_frame;
  unit_frame++;
  
  routine_frame->name = EVAL_FUNCTION_NAME;
  routine_frame->hash = eval_id;
  routine_frame->cfm.dataset = NULL;
  routine_frame->cfm.cfg = routine_cfg_new(current_unit->hash, routine_frame->hash);
  current_routine = routine_frame;
  routine_frame++;
}

const char *get_function_declaration_path(const char *function_name)
{
  function_fqn_t *fqn;

  fqn = (function_fqn_t *) sctable_lookup(&routine_table, hash_string(function_name));
  if (fqn == NULL)
    return "unknown";
  else
    return fqn->unit.path;
}

control_flow_metadata_t *get_cfm(const char *function_name)
{
  function_fqn_t *fqn;

  fqn = (function_fqn_t *) sctable_lookup(&routine_table, hash_string(function_name));
  if (fqn == NULL)
    return NULL;
  else
    return &fqn->function.cfm;
}

void add_compiled_op(const zend_op *op, uint index)
{
  routine_cfg_assign_opcode(current_routine->cfm.cfg, op->opcode, op->extended_value, index);
  
  switch (op->opcode) {
    case ZEND_JMP:
    case ZEND_RETURN:
      break;
    default:
      add_compiled_edge(current_routine->cfm.cfg->opcode_count, current_routine->cfm.cfg->opcode_count+1);
  }
  
  current_routine->index = MAX(current_routine->index, index);
}

void add_compiled_edge(uint from_index, uint to_index)
{
  routine_cfg_add_edge(current_routine->cfm.cfg, from_index, to_index);
  
  PRINT("[emit %d->%d for {%s|%s, 0x%x|0x%x}]\n", from_index, to_index,
        get_compilation_unit_path(), get_compilation_routine_name(),
        get_compilation_unit_hash(), get_compilation_routine_hash());
  
  if (current_routine->cfm.dataset == NULL)
    write_op_edge(current_unit->hash, current_routine->hash, from_index, to_index);
  else
    dataset_routine_verify_compiled_edge(current_routine->cfm.dataset, from_index, to_index);
}
