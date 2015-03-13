#include "php.h"
#include "lib/script_cfi_utils.h"
#include "lib/script_cfi_hashtable.h"
#include "cfg.h"
#include "event_handler.h"
#include "cfg_handler.h"
#include "dataset.h"
#include "metadata_handler.h"
#include "operand_resolver.h"
#include "compile_context.h"

#define ROUTINE_NAME_LENGTH 256
#define CLOSURE_NAME "{closure}"
#define CLOSURE_NAME_LENGTH 9
#define CLOSURE_NAME_TEMPLATE "<closure-%d>"

// #define SPOT_DEBUG 1

typedef struct _compilation_unit_t {
  const char *site_root; // ends in '/'
  const char *path;
} compilation_unit_t;

typedef struct _compilation_routine_t {
  uint hash;
  control_flow_metadata_t cfm;
} compilation_routine_t;

typedef struct _function_fqn_t {
  compilation_unit_t unit;
  compilation_routine_t function;
} function_fqn_t;

static sctable_t routines_by_hash;
static sctable_t routines_by_opcode_address;
static uint closure_count = 0; // must be per compilation unit!

static control_flow_metadata_t last_eval_cfm = { "<uninitialized>", NULL, NULL };

extern cfg_t *app_cfg;

inline int is_closure(const char *function_name)
{
  const char *closure_position;
  uint len = strlen(function_name);
  if (len < CLOSURE_NAME_LENGTH)
    return -1;
  closure_position = function_name + len - CLOSURE_NAME_LENGTH;
  if (strcmp(closure_position, CLOSURE_NAME) == 0)
    return (closure_position - function_name);
  else
    return -1;
}

void init_compile_context()
{
  routines_by_hash.hash_bits = 8;
  sctable_init(&routines_by_hash);

  routines_by_opcode_address.hash_bits = 8;
  sctable_init(&routines_by_opcode_address);
}

void function_compiled(zend_op_array *op_array)
{
  uint i, eval_id;
  bool is_script_body = false, is_eval = false;
  bool has_routine_name = false, is_already_compiled = false;
  function_fqn_t *fqn;
  control_flow_metadata_t cfm = { "<uninitialized>", NULL, NULL };
  const char *function_name;
  char *buffer, *filename, *site_filename, routine_name[ROUTINE_NAME_LENGTH];
#ifdef SPOT_DEBUG
  bool spot = false;
#endif

  if (op_array->type == ZEND_EVAL_CODE) { // lambda or plain eval?
    is_eval = true;
    eval_id = get_next_eval_id();
    sprintf(routine_name, "<eval-%d>", eval_id);
    has_routine_name = true;
  } else if (op_array->type == ZEND_USER_FUNCTION) {
    if (op_array->function_name == NULL) {
      function_name = "<script-body>";
      is_script_body = true;
    } else {
      int closure_index = is_closure(op_array->function_name->val);
      if (closure_index >= 0) {
        strncpy(routine_name, op_array->function_name->val, closure_index);
        sprintf(routine_name + closure_index, CLOSURE_NAME_TEMPLATE, closure_count++);
        has_routine_name = true;
      } else {
        function_name = op_array->function_name->val;
        if (strcmp(function_name, "__lambda_func") == 0) {
          is_eval = true;
          eval_id = get_next_eval_id();
          sprintf(routine_name, "<default>:lambda_%d", EG(lambda_count)+1);
          has_routine_name = true;
        }
      }
    }
  } else {
    PRINT("Warning: skipping unrecognized op_array of type %d\n", op_array->type);
    return;
  }

  if (has_routine_name) {
    fqn = malloc(sizeof(function_fqn_t));
    fqn->unit.path = "<eval>";

    fqn->unit.site_root = locate_site_root(op_array->filename->val);
    if (fqn->unit.site_root == NULL) {
      WARN("Cannot find site root for eval %s. Defaulting to '/'\n", op_array->filename->val);
      fqn->unit.site_root = "/";
    }
  } else {
    const char *classname;
    bool free_filename = false;

    if (strncmp(op_array->filename->val, "phar://", 7) == 0) {
      filename = op_array->filename->val;
    } else {
      filename = zend_resolve_path(op_array->filename->val, op_array->filename->len);
      if (filename == NULL) {
        ERROR("Cannot resolve compilation unit filename %s for op_array->type = %d\n",
              op_array->filename->val, op_array->type);
        return;
      }
      free_filename = true;
    }
    fqn = malloc(sizeof(function_fqn_t));
    fqn->unit.site_root = locate_site_root(filename);
    if (fqn->unit.site_root == NULL) {
      WARN("Cannot find site root for filename %s. Defaulting to '/'\n", filename);
      fqn->unit.site_root = "/";
    }
    site_filename = filename + strlen(fqn->unit.site_root);
    buffer = malloc(strlen(site_filename) + 1);
    strcpy(buffer, site_filename);
    fqn->unit.path = buffer;
    buffer = NULL;
    if (free_filename)
      efree(filename);
    filename = site_filename = NULL;

    if (is_script_body) {
      classname = fqn->unit.path;
    } else {
      if (op_array->scope == NULL)
        classname = "<default>";
      else
        classname = op_array->scope->name->val;
    }
    sprintf(routine_name, "%s:%s", classname, function_name);
    has_routine_name = true;
  }

  buffer = malloc(strlen(routine_name) + 1);
  strcpy(buffer, routine_name);
  cfm.routine_name = buffer;
  buffer = NULL;
  if (is_eval)
    fqn->function.hash = hash_eval(eval_id);
  else
    fqn->function.hash = hash_routine(cfm.routine_name);

  if (!is_eval) {
    cfm.dataset = dataset_routine_lookup(fqn->function.hash);
    cfm.cfg = cfg_routine_lookup(app_cfg, fqn->function.hash);
  }
  if (cfm.cfg == NULL) {
    cfm.cfg = routine_cfg_new(fqn->function.hash); // crash in eval-function-test.php
    cfg_add_routine(app_cfg, cfm.cfg);
    write_routine_catalog_entry(fqn->function.hash, fqn->unit.path, routine_name);
  } else {
    zend_op *op;
    cfg_opcode_t *cfg_opcode;
    bool recompile = false;
    //cfg_opcode_edge_t *cfg_edge; // skipping edges for now
    compiled_edge_target_t target;
    for (i = 0; i < op_array->last; i++) {
      op = &op_array->opcodes[i];
      if (zend_get_opcode_name(op->opcode) == NULL)
        continue;
      cfg_opcode = routine_cfg_get_opcode(cfm.cfg, i);
      //cfg_edge = routine_cfg_get_opcode_edge(cfm.cfg, i);

      if (op->opcode != cfg_opcode->opcode ||
          op->extended_value != cfg_opcode->extended_value) {
        recompile = true;
        break;
      }
      //target = get_compiled_edge_target(op, i);
    }
    is_already_compiled = !recompile;
  }
  fqn->function.cfm = cfm;

#ifdef SPOT_DEBUG
  spot = (fqn->function.hash == 0x933ca2cd);
#endif

  if (cfm.dataset == NULL) {
    WARN("--- Function compiled from %d opcodes at "PX": %s|%s: 0x%x"
         " (dataset not found)\n",
         op_array->last, p2int(op_array->opcodes), fqn->unit.path, routine_name,
         fqn->function.hash);
  } else {
    PRINT("--- Function compiled from %d opcodes at "PX": %s|%s: 0x%x"
          " (dataset found)\n",
          op_array->last, p2int(op_array->opcodes), fqn->unit.path, routine_name,
          fqn->function.hash);
  }

  sctable_add_or_replace(&routines_by_hash, fqn->function.hash, fqn);
  sctable_add_or_replace(&routines_by_opcode_address,
                         hash_addr(op_array->opcodes), fqn);

  if (is_already_compiled) {
    PRINT("(skipping existing routine 0x%x at "PX")\n", fqn->function.hash,
          p2int(op_array->opcodes));
    fflush(stderr);
    return; // verify equal?
  }

  for (i = 0; i < op_array->last; i++) {
    compiled_edge_target_t target;
    zend_op *op = &op_array->opcodes[i];
    cfg_node_t from_node = { op->opcode, i };
    if (zend_get_opcode_name(op->opcode) == NULL)
      continue;

#ifdef SPOT_DEBUG
    if (spot) {
      SPOT("Compiling opcode 0x%x at index %d of %s (0x%x)\n",
           op->opcode, i, routine_name, fqn->function.hash);
    }
#endif

    if (is_static_analysis()) {
      uint to_routine_hash;
      switch (op->opcode) {
        case ZEND_INCLUDE_OR_EVAL: {
          if (op->op1_type == IS_CONST) {
            switch (op->extended_value) {
              case ZEND_INCLUDE:
              case ZEND_REQUIRE:
              case ZEND_INCLUDE_ONCE:
              case ZEND_REQUIRE_ONCE: {
                const char *site_to_path, *internal_to_path = resolve_constant_include(op);
                char to_unit_path[ROUTINE_NAME_LENGTH], to_routine_name[ROUTINE_NAME_LENGTH];
                bool free_internal_to_path = false;

                // make the included path absolute
                if (internal_to_path[0] == '/') {
                  strcpy(to_unit_path, internal_to_path);
                } else { // `internal_to_path` is just the to filename
                  char *to_unit_filename;
                  strcpy(to_unit_path, fqn->unit.site_root);
                  strcat(to_unit_path, fqn->unit.path);
                  to_unit_filename = strrchr(to_unit_path, '/');
                  if (to_unit_filename == NULL) {
                    to_unit_filename = to_unit_path;
                  } else {
                    to_unit_filename++;
                    to_unit_filename[0] = '\0'; // truncate to remove the from filename
                  }
                  strcat(to_unit_path, internal_to_path); // `internal_to_path` is just the to filename
                }
                free((char *) internal_to_path);

                internal_to_path = zend_resolve_path(to_unit_path, strlen(to_unit_path));
                if (internal_to_path == NULL) {
                  internal_to_path = to_unit_path;
                } else {
                  free_internal_to_path = true;
                }
                site_to_path = internal_to_path + strlen(fqn->unit.site_root);
                sprintf(to_routine_name, "%s:<script-body>", site_to_path);

                if (free_internal_to_path)
                  efree((char *) internal_to_path);
                internal_to_path = site_to_path = NULL;

                to_routine_hash = hash_routine(to_routine_name);

                WARN("Opcode %d includes %s (0x%x)\n", i, to_routine_name, to_routine_hash);

                write_routine_edge(fqn->function.hash, i,
                                   to_routine_hash, 0, USER_LEVEL_BOTTOM);
              } break;
              case ZEND_EVAL: {
                char *eval_body = resolve_eval_body(op);
                PRINT("Opcode %d calls eval(%s)\n", i, eval_body);
                free(eval_body);
              } break;
            }
          }
        } break;
        case ZEND_NEW: {
          if (op->op1_type == IS_CONST)
            PRINT("Opcode %d calls new %s()\n", i, Z_STRVAL_P(op->op1.zv));
        } break;
        case ZEND_INIT_FCALL_BY_NAME:
        case ZEND_INIT_METHOD_CALL: {
          if (op->op2_type == IS_CONST) {
            const char *classname;
            char routine_name[ROUTINE_NAME_LENGTH];

            if (op_array->scope == NULL)
              classname = "<default>";
            else
              classname = op_array->scope->name->val;
            sprintf(routine_name, "%s:%s", classname, Z_STRVAL_P(op->op2.zv));
            to_routine_hash = hash_routine(routine_name);

            SPOT("Opcode %d calls function %s (0x%x)\n", i, routine_name, to_routine_hash);

            write_routine_edge(fqn->function.hash, i,
                               to_routine_hash, 0, USER_LEVEL_BOTTOM);
          }
        } break;
      }
    }

    routine_cfg_assign_opcode(cfm.cfg, op->opcode, op->extended_value, i);
    target = get_compiled_edge_target(op, i);
    if (target.type == COMPILED_EDGE_DIRECT) {
      if (target.index >= op_array->last) {
        ERROR("Skipping foobar edge %u|0x%x(%u,%u) -> %u in {%s|%s, 0x%x}\n",
              i, op->opcode, op->op1_type, op->op2_type, target.index,
              fqn->unit.path, fqn->function.cfm.routine_name,
              fqn->function.hash);
        continue;
      }

#ifdef SPOT_DEBUG
      if (spot) {
        SPOT("Compiling opcode edge %d -> %d for opcode 0x%x in %s (0x%x)\n",
             i, target.index, op->opcode, routine_name, fqn->function.hash);
      }
#endif

      routine_cfg_add_opcode_edge(cfm.cfg, i, target.index, USER_LEVEL_COMPILER);
      PRINT("\t[create edge %u|0x%x(%u,%u) -> %u for {%s|%s, 0x%x}]\n",
            i, op->opcode, op->op1_type, op->op2_type, target.index,
            fqn->unit.path, fqn->function.cfm.routine_name,
            fqn->function.hash);
    }
  }

  if (is_eval) {
    dataset_match_eval(&cfm);
    last_eval_cfm = cfm;
  }

  if (cfm.dataset == NULL) {
    cfg_opcode_t *cfg_opcode;
    cfg_opcode_edge_t *cfg_edge;
    for (i = 0; i < cfm.cfg->opcodes.size; i++) {
      cfg_opcode = routine_cfg_get_opcode(cfm.cfg, i);
#ifdef SPOT_DEBUG
      if (spot) {
        SPOT("\t[emit %s at %d for {%s|%s, 0x%x}]\n",
              zend_get_opcode_name(cfg_opcode->opcode), i,
              fqn->unit.path, fqn->function.cfm.routine_name,
              fqn->function.hash);
      }
#endif
      write_node(fqn->function.hash, cfg_opcode, i);
    }
    for (i = 0; i < cfm.cfg->opcode_edges.size; i++) {
      cfg_edge = routine_cfg_get_opcode_edge(cfm.cfg, i);
      write_op_edge(fqn->function.hash, cfg_edge->from_index,
                    cfg_edge->to_index, USER_LEVEL_COMPILER);
#ifdef SPOT_DEBUG
      if (spot) {
        SPOT("\t[emit %d -> %d in 0x%x]\n", cfg_edge->from_index,
             cfg_edge->to_index, fqn->function.hash);
      }
#endif
    }
    WARN("No dataset for routine 0x%x\n", fqn->function.hash);
  } else if (is_eval_routine(fqn->function.hash)) {
    cfg_opcode_edge_t *cfg_edge;
    for (i = 0; i < cfm.cfg->opcodes.size; i++) {
      dataset_routine_verify_opcode(cfm.dataset, i,
                                    routine_cfg_get_opcode(cfm.cfg, i)->opcode);
    }
    for (i = 0; i < cfm.cfg->opcode_edges.size; i++) {
      cfg_edge = routine_cfg_get_opcode_edge(cfm.cfg, i);
      dataset_routine_verify_compiled_edge(cfm.dataset, cfg_edge->from_index, cfg_edge->to_index);
    }
    WARN("Found dataset for routine 0x%x\n", fqn->function.hash);
  }

  flush_all_outputs();
}

// (can never find an eval this way)
control_flow_metadata_t *get_cfm_by_name(const char *routine_name)
{
  function_fqn_t *fqn;

  fqn = (function_fqn_t *) sctable_lookup(&routines_by_hash, hash_routine(routine_name));
  if (fqn == NULL)
    return NULL;
  else
    return &fqn->function.cfm;
}

control_flow_metadata_t *get_cfm_by_opcodes_address(zend_op *opcodes)
{
  function_fqn_t *fqn;

  fqn = (function_fqn_t *) sctable_lookup(&routines_by_opcode_address, hash_addr(opcodes));
  if (fqn == NULL)
    return NULL;
  else
    return &fqn->function.cfm;
}

control_flow_metadata_t get_last_eval_cfm()
{
  return last_eval_cfm;
}

compiled_edge_target_t get_compiled_edge_target(zend_op *op, uint op_index)
{
  compiled_edge_target_t target = { COMPILED_EDGE_NONE, COMPILED_EDGE_UNKNOWN_TARGET };

  switch (op->opcode) {
    case ZEND_JMP:
      target.type = COMPILED_EDGE_DIRECT;
      target.index = op_index + ((zend_op *)op->op1.jmp_addr - op);
      break;
    case ZEND_JMPZ:
    case ZEND_JMPNZ:
    case ZEND_JMPZNZ:
    case ZEND_JMPZ_EX:
    case ZEND_JMPNZ_EX:
    case ZEND_FE_RESET:
    case ZEND_FE_FETCH:
      target.type = COMPILED_EDGE_DIRECT;
      target.index = op_index + ((zend_op *)op->op2.jmp_addr - op);
      break;
    case ZEND_DO_FCALL:
    case ZEND_INCLUDE_OR_EVAL:
      target.type = COMPILED_EDGE_CALL;
      break;
    case ZEND_THROW:
    case ZEND_BRK:
    case ZEND_CONT:
      target.type = COMPILED_EDGE_INDIRECT;
      break;
    case ZEND_NEW:
      target.type = COMPILED_EDGE_DIRECT;
      target.index = op_index + 2;
      break;
  }

  return target;
}
