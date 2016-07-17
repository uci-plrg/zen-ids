#include "php.h"
#include "lib/script_cfi_utils.h"
#include "lib/script_cfi_hashtable.h"
#include "cfg.h"
#include "event_handler.h"
#include "cfg_handler.h"
#include "dataset.h"
#include "metadata_handler.h"
#include "operand_resolver.h"
#include "dataflow.h"
#include "interp_context.h"
#include "compile_context.h"

#define CLOSURE_NAME "{closure}"
#define CLOSURE_NAME_LENGTH 9
#define CLOSURE_ID_TEMPLATE "<closure-%d>"
#define CLOSURE_ID_LENGTH 256
#define MAX_STACK_FRAME_fcall_stack 0x100

// #define SPOT_DEBUG 1

typedef struct _compilation_unit_t {
  application_t *application; // `application.path` ends in '/'
  const char *path;
} compilation_unit_t;

typedef struct _compilation_routine_t {
  uint caller_hash; /* <scope>:<function-name> */
  uint callee_hash; /* <path>:<line-number>:<function-name> */
  control_flow_metadata_t cfm;
} compilation_routine_t;

typedef struct _function_fqn_t {
  compilation_unit_t unit;
  compilation_routine_t function;
} function_fqn_t; // mem: leaking the outer shell here

/* N.B.: in general, fcall stacks are unsafe at runtime because of tricky cases
 * like autoloaders that invoke PHP functions without any associated call. It
 * seems to mostly work for static analysis though.
 */
typedef struct _fcall_init_t {
  uint init_index;
  uint routine_hash;
  const char *routine_name;
  zend_uchar opcode;
} fcall_init_t;

fcall_init_t fcall_stack[MAX_STACK_FRAME_fcall_stack];
fcall_init_t *fcall_frame;

static sctable_t routines_by_callee_hash;
static sctable_t routines_by_opcode_address;
static uint closure_count = 0;

static sctable_t handler_table;
//static uint handler_index = 0;

#define BUILTIN_ROUTINE_HASH_PLACEHOLDER 1

static control_flow_metadata_t last_eval_cfm = { "<uninitialized>", NULL, NULL };

static inline void push_fcall_init(application_t *app, uint index, zend_uchar opcode,
                                   uint routine_hash, const char *routine_name)
{
  // SPOT("Opcode %d prepares call to %s (0x%x)\n", index, routine_name, routine_hash);

  INCREMENT_STACK(fcall_stack, fcall_frame);
  fcall_frame->init_index = index;
  fcall_frame->routine_hash = routine_hash;
  fcall_frame->routine_name = request_strdup(routine_name); // mem: per request?
  fcall_frame->opcode = opcode;

  push_dataflow_fcall_init(app, routine_hash, routine_name);
}

static inline void push_fcall_skip(uint index, zend_uchar opcode)
{
  INCREMENT_STACK(fcall_stack, fcall_frame);
  fcall_frame->init_index = index;
  fcall_frame->routine_hash = 0;
  fcall_frame->routine_name = NULL;
  fcall_frame->opcode = opcode;
}

static inline bool has_fcall_init(uint index)
{
  return fcall_frame->init_index == index && fcall_frame->routine_hash > 0;
}

static inline fcall_init_t *pop_fcall_init()
{
  fcall_init_t *top = fcall_frame;
  DECREMENT_STACK(fcall_stack, fcall_frame); // alpha: hitting bottom
  return top;
}

static inline fcall_init_t *peek_fcall_init()
{
  return fcall_frame;
}

static inline void reset_fcall_stack()
{
  fcall_frame = fcall_stack + 1;
}

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
  routines_by_callee_hash.hash_bits = 9;
  sctable_init(&routines_by_callee_hash);

  routines_by_opcode_address.hash_bits = 9;
  sctable_init(&routines_by_opcode_address);

  handler_table.hash_bits = 9;
  sctable_init(&handler_table);

  memset(fcall_stack, 0, sizeof(fcall_init_t));
}

static function_fqn_t *register_new_function(zend_op_array *op_array)
{
  zend_op *op, *end;
  uint i, eval_id;
  bool is_script_body = false, is_eval = false;
  bool has_routine_name = false, is_already_compiled = false;
  function_fqn_t *fqn = PROCESS_NEW(function_fqn_t); /* can use REQUEST_NEW w/o opcache */
  control_flow_metadata_t cfm = { "<uninitialized>", NULL, NULL, NULL };
  const char *function_name;
  char *buffer, *site_filename;
  zend_string *filename;
  char routine_name[ROUTINE_NAME_LENGTH], routine_caller_name[ROUTINE_NAME_LENGTH] = {0};
  char closure_id[CLOSURE_ID_LENGTH];
#ifdef SPOT_DEBUG
  bool spot = false;
#endif

  /******************************** 60s ************************************/

  if (op_array->type == ZEND_EVAL_CODE) { // lambda or plain eval?
    is_eval = true;
    eval_id = get_next_eval_id();
    sprintf(routine_name, "<eval-%d>", eval_id);
    has_routine_name = true;
  } else if (op_array->type == ZEND_USER_FUNCTION) {
    if (op_array->function_name == NULL) {
      function_name = "<script-body>";
      is_script_body = true;
      closure_count = 0;
    } else {
      int closure_index = is_closure(op_array->function_name->val);
      if (closure_index >= 0) {
        strncpy(closure_id, op_array->function_name->val, closure_index);
        sprintf(closure_id + closure_index, CLOSURE_ID_TEMPLATE, closure_count++);
        function_name = closure_id;
      } else {
        function_name = op_array->function_name->val;
        if (strcmp(function_name, "__lambda_func") == 0) {
          is_eval = true;
          eval_id = get_next_eval_id();
          sprintf(routine_name, COMPILED_ROUTINE_DEFAULT_SCOPE":lambda_%d", EG(lambda_count)+1);
          has_routine_name = true;
        }
      }
    }
  } else {
    PRINT("Warning: skipping unrecognized op_array of type %d\n", op_array->type);
    // PROCESS_FREE(fqn);
    return NULL;
  }

  if (has_routine_name) {
    fqn->unit.path = "<eval>";
    fqn->unit.application = locate_application(op_array->filename->val);
  } else {
    bool free_filename = false;

    if (strncmp(op_array->filename->val, "phar://", 7) == 0) {
      filename = op_array->filename;
    } else {
      filename = zend_resolve_path(op_array->filename->val, op_array->filename->len);
      if (filename == NULL) {
        ERROR("Cannot resolve compilation unit filename %s for op_array->type = %d\n",
              op_array->filename->val, op_array->type);
        return NULL;
      }
      free_filename = true;
    }
    fqn->unit.application = locate_application(filename->val);
    site_filename = filename->val + strlen(fqn->unit.application->root);
    buffer = PROCESS_ALLOC(strlen(site_filename) + 1);
    strcpy(buffer, site_filename);
    fqn->unit.path = buffer;
    buffer = NULL;
    if (free_filename)
      zend_string_release(filename);
    filename = NULL;
    site_filename = NULL;

    if (is_script_body) {
      sprintf(routine_name, "%s:%s", fqn->unit.path, function_name);
    } else if (op_array->scope == NULL) {
      sprintf(routine_name, "%s:%d:%s", fqn->unit.path, op_array->opcodes[0].lineno, function_name);
      sprintf(routine_caller_name, "%s:%s", COMPILED_ROUTINE_DEFAULT_SCOPE, function_name);
    } else {
      sprintf(routine_name, "%s:%s", op_array->scope->name->val, function_name);
    }
    has_routine_name = true;
  }

  /******************************** 62s ************************************/

  if (strlen(routine_caller_name) == 0)
    strcat(routine_caller_name, routine_name);

  cfm.app = fqn->unit.application;

  if (is_eval) {
    uint eval_routine_hash = dataset_match_eval(&cfm, op_array);

    if (eval_routine_hash == 0) {
      cfm.cfg = cfg_get_matching_eval(cfm.app->cfg, op_array);
      if (cfm.cfg == NULL)
        fqn->function.callee_hash = fqn->function.caller_hash = hash_eval(eval_id);
      else
        fqn->function.callee_hash = fqn->function.caller_hash = cfm.cfg->routine_hash;
    } else {
      fqn->function.callee_hash = fqn->function.caller_hash = eval_routine_hash;
      cfm.cfg = cfg_routine_lookup(cfm.app->cfg, eval_routine_hash);
    }

    if (cfm.cfg == NULL) {
      cfm.cfg = routine_cfg_new(fqn->function.callee_hash);
      cfg_add_routine(cfm.app->cfg, cfm.cfg);
      cfg_add_eval(cfm.app->cfg, cfm.cfg);
      write_routine_catalog_entry(cfm.app, fqn->function.callee_hash, fqn->unit.path, routine_name);
    } else {
      is_already_compiled = true;
      eval_id = get_eval_id(cfm.cfg->routine_hash);
    }
    sprintf(routine_name, "<eval-%d>", eval_id);
  } else {
    fqn->function.callee_hash = hash_routine(routine_name);
    fqn->function.caller_hash = hash_routine(routine_caller_name);

    cfm.dataset = dataset_routine_lookup(cfm.app, fqn->function.callee_hash);
    cfm.cfg = cfg_routine_lookup(cfm.app->cfg, fqn->function.callee_hash);

    if (cfm.cfg == NULL) {
      cfm.cfg = routine_cfg_new(fqn->function.callee_hash);
      cfg_add_routine(cfm.app->cfg, cfm.cfg);
      write_routine_catalog_entry(cfm.app, fqn->function.callee_hash, fqn->unit.path, routine_name);
    } else {
      cfg_opcode_t *cfg_opcode;
      bool recompile = false;
      uint i;

      end = &op_array->opcodes[op_array->last];
      for (i = 0, op = op_array->opcodes; op < end; i++, op++) {
        if (zend_get_opcode_name(op->opcode) == NULL)
          continue;

        cfg_opcode = routine_cfg_get_opcode(cfm.cfg, i);

        if (op->opcode != cfg_opcode->opcode ||
            op->extended_value != cfg_opcode->extended_value) {
          recompile = true;
          SPOT("Recompile %s\n", routine_name);
          break;
        }
      }
      is_already_compiled = !recompile;
    }
  }

  /******************************** 66s ************************************/

  buffer = PROCESS_ALLOC(strlen(routine_name) + 1);
  strcpy(buffer, routine_name);
  cfm.routine_name = buffer;
  buffer = NULL;
  fqn->function.cfm = cfm;

  if (is_eval)
    last_eval_cfm = cfm;

#ifdef SPOT_DEBUG
  spot = (fqn->function.callee_hash == 0x933ca2cd);
#endif

  if (cfm.dataset == NULL) {
    WARN("--- Function compiled from %d opcodes at "PX": %s|%s: 0x%x"
         " (dataset not found)\n",
         op_array->last, p2int(op_array->opcodes), fqn->unit.path, routine_name,
         fqn->function.callee_hash);
  } else {
    PRINT("--- Function compiled from %d opcodes at "PX": %s|%s: 0x%x"
          " (dataset found)\n",
          op_array->last, p2int(op_array->opcodes), fqn->unit.path, routine_name,
          fqn->function.callee_hash);
  }

  /******************************** 66s ************************************/

  sctable_add_or_replace(&routines_by_callee_hash, fqn->function.callee_hash, fqn);
  PRINT("Installing hashcodes for %s (0x%x) under hash 0x%llx\n", routine_name,
       fqn->function.caller_hash, hash_addr(op_array->opcodes));
  sctable_add_or_replace(&routines_by_opcode_address, hash_addr(op_array->opcodes), fqn);

  if (is_already_compiled) {
    //SPOT("<compile> skipping existing routine %s (0x%x at "PX")\n", cfm.routine_name,
    //     fqn->function.callee_hash, p2int(op_array->opcodes));
    fflush(stderr);
    return fqn; // verify equal?
  }

  /*************************** 68s (as written) ******************************/

  // SPOT("<compile> compiling routine %s (0x%x at "PX")\n", cfm.routine_name,
  //     fqn->function.callee_hash, p2int(op_array->opcodes));

  if (IS_OPCODE_DUMP_ENABLED()) {
    if (is_script_body)
      dump_script_header(cfm.app, routine_name, fqn->function.callee_hash);
    else
      dump_function_header(cfm.app, fqn->unit.path, routine_name, fqn->function.callee_hash);
  }

  if (is_static_analysis() || is_dataflow_analysis() || IS_OPCODE_DUMP_ENABLED())
    reset_fcall_stack();
  if (is_dataflow_analysis()) {
    add_dataflow_routine(fqn->unit.application, fqn->function.caller_hash, op_array,
                         !(is_script_body || is_eval));
  }

  end = &op_array->opcodes[op_array->last];
  for (i = 0, op = op_array->opcodes; op < end; i++, op++) {
    compiled_edge_target_t target;
    //cfg_node_t from_node = { op->opcode, i };
    sink_identifier_t sink_id = {NULL};

    PRINT("Compiling opcode 0x%x at index %d of %s (0x%x)\n",
          op->opcode, i, routine_name, fqn->function.callee_hash);

    if (IS_OPCODE_DUMP_ENABLED()) {
      fcall_init_t *fcall;

      switch (op->opcode) {
        case ZEND_DO_FCALL:
          fcall = peek_fcall_init();
          if (fcall->routine_name != NULL)
            dump_fcall_opcode(cfm.app, op_array, op, fcall->routine_name);
          if (fcall->routine_hash > BUILTIN_ROUTINE_HASH_PLACEHOLDER)
            sink_id.call_target = fcall->routine_name;
          break;
        case ZEND_SEND_VAL:
        case ZEND_SEND_VAL_EX:
        case ZEND_SEND_VAR:
        case ZEND_SEND_VAR_NO_REF:
        case ZEND_SEND_REF:
        case ZEND_SEND_VAR_EX:
        case ZEND_SEND_UNPACK:
        case ZEND_SEND_ARRAY:
        case ZEND_SEND_USER:
          fcall = peek_fcall_init();
          dump_fcall_arg(cfm.app, op_array, op, fcall->routine_name);
          sink_id.call_target = fcall->routine_name;
          break;
        case ZEND_ASSIGN_OBJ:
        case ZEND_ASSIGN_DIM:
          dump_map_assignment(cfm.app, op_array, op, &op_array->opcodes[i+1]);
          break;
        case ZEND_FE_FETCH_R:
          dump_foreach_fetch(cfm.app, op_array, op, &op_array->opcodes[i+1]);
          break;
        default:
          dump_opcode(cfm.app, op_array, op);
      }

      if (sink_id.call_target != NULL)
        identify_sink_operands(cfm.app, op_array, op, sink_id);
    }

    if (is_dataflow_analysis()) {
      fcall_init_t *fcall;

      switch (op->opcode) {
        case ZEND_DO_FCALL:
          fcall = peek_fcall_init();
          if (fcall->routine_hash > BUILTIN_ROUTINE_HASH_PLACEHOLDER)
            add_dataflow_fcall(cfm.app, fqn->function.caller_hash, i, op_array, fcall->routine_name);
          //sink_id.call_target = fcall->routine_name;
          break;
        case ZEND_SEND_VAL:
        case ZEND_SEND_VAL_EX:
        case ZEND_SEND_VAR:
        case ZEND_SEND_VAR_NO_REF:
        case ZEND_SEND_REF:
        case ZEND_SEND_VAR_EX:
        case ZEND_SEND_UNPACK:
        case ZEND_SEND_ARRAY:
        case ZEND_SEND_USER:
          fcall = peek_fcall_init();
          if (fcall->routine_hash > BUILTIN_ROUTINE_HASH_PLACEHOLDER)
            add_dataflow_fcall_arg(fqn->function.caller_hash, i, op_array, fcall->routine_name);
          //sink_id.call_target = fcall->routine_name;
          break;
        case ZEND_ASSIGN_OBJ:
        case ZEND_ASSIGN_DIM:
          add_dataflow_map_assignment(fqn->function.caller_hash, i, op_array);
          break;
        case ZEND_FE_FETCH_R:
          add_dataflow_foreach_fetch(fqn->function.caller_hash, i, op_array);
          break;
        default:
          add_dataflow_opcode(fqn->function.caller_hash, i, op_array);
      }

      //add_dataflow_sinks(fqn->function.hash, op, sink_id);
    }

    if (zend_get_opcode_name(op->opcode) == NULL)
      continue;

    if (is_static_analysis() || is_dataflow_analysis() || IS_OPCODE_DUMP_ENABLED()) {
      uint to_routine_hash;
      const char *classname;
      bool ignore_call = false;

      switch (op->opcode) {
        case ZEND_INCLUDE_OR_EVAL: {
          if ((is_static_analysis() || is_dataflow_analysis()) && op->op1_type == IS_CONST) {
            switch (op->extended_value) {
              case ZEND_INCLUDE:
              case ZEND_REQUIRE:
              case ZEND_INCLUDE_ONCE:
              case ZEND_REQUIRE_ONCE: {
                const char *site_to_path, *internal_to_path = resolve_constant_include(op_array, op);
                zend_string *resolved_internal_to_path = NULL;
                char to_unit_path[ROUTINE_NAME_LENGTH], to_routine_name[ROUTINE_NAME_LENGTH];

                // make the included path absolute
                if (internal_to_path[0] == '/') {
                  strcpy(to_unit_path, internal_to_path);
                } else { // `internal_to_path` is just the to filename
                  char *to_unit_filename;
                  strcpy(to_unit_path, fqn->unit.application->root);
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
                // PROCESS_FREE((char *) internal_to_path); // mem: per-request?

                resolved_internal_to_path = zend_resolve_path(to_unit_path, strlen(to_unit_path));
                if (resolved_internal_to_path == NULL) {
                  internal_to_path = to_unit_path;
                } else {
                  internal_to_path = resolved_internal_to_path->val;
                }
                site_to_path = internal_to_path + strlen(fqn->unit.application->root);
                sprintf(to_routine_name, "%s:<script-body>", site_to_path);
                to_routine_hash = hash_routine(to_routine_name);

                if (is_dataflow_analysis()) {
                  add_dataflow_include(fqn->function.caller_hash, i, op_array, internal_to_path,
                                       to_routine_hash);
                }

                if (resolved_internal_to_path != NULL) {
                  zend_string_release(resolved_internal_to_path);
                  resolved_internal_to_path = NULL;
                }
                internal_to_path = site_to_path = NULL;

                WARN("Opcode %d includes %s (0x%x)\n", i, to_routine_name, to_routine_hash);

                write_request_edge(true, cfm.app, fqn->function.callee_hash, i,
                                   to_routine_hash, 0, USER_LEVEL_TOP);
                write_routine_edge(cfm.app, fqn->function.callee_hash, i,
                                   to_routine_hash, 0, USER_LEVEL_TOP);
              } break;
              case ZEND_EVAL: {
                char *eval_body = resolve_eval_body(op_array, op);
                PRINT("Opcode %d calls eval(%s)\n", i, eval_body);
                // PROCESS_FREE(eval_body); // mem: per-request?
              } break;
            }
          }
        } break;
        case ZEND_NEW: {
          if (op->op1_type == IS_CONST) {
            PRINT("Opcode %d calls new %s()\n", i, Z_STRVAL_P(RT_CONSTANT(op_array, op->op1)));
            classname = Z_STRVAL_P(RT_CONSTANT(op_array, op->op1));
          } else {
            classname = "<unknown-type>";
          }

          sprintf(routine_name, "new %s", classname);
          to_routine_hash = hash_routine(routine_name);
          push_fcall_init(cfm.app, i, op->opcode, to_routine_hash, routine_name);
        } break;
        case ZEND_DO_FCALL: {
          fcall_init_t *fcall = pop_fcall_init();
          if (fcall->routine_hash > BUILTIN_ROUTINE_HASH_PLACEHOLDER) {
            PRINT("Opcode %d calls function 0x%x\n", i, fcall->routine_hash);
            if (is_static_analysis()) {
              write_request_edge(true, cfm.app, fqn->function.callee_hash, i,
                                 fcall->routine_hash, 0, USER_LEVEL_TOP);
              write_routine_edge(cfm.app, fqn->function.callee_hash, i,
                                 fcall->routine_hash, 0, USER_LEVEL_TOP);
            }
          } else if (fcall->routine_hash == 0 && fcall->opcode > 0) {
            PRINT("Unresolved routine edge at index %d (op 0x%x) in %s at %s:%d (0x%x). "
                 "Dataset: %s. edges: %d.\n", i, fcall->opcode, cfm.routine_name, fqn->unit.path,
                 op->lineno, fqn->function.callee_hash, cfm.dataset == NULL ? "missing" : "found",
                 cfm.dataset == NULL ? 0 : dataset_get_call_target_count(cfm.app, cfm.dataset, i));
          }
          // PROCESS_FREE((char *) fcall->routine_name);
        } break;
        case ZEND_INIT_FCALL:
        case ZEND_INIT_FCALL_BY_NAME:
        case ZEND_INIT_NS_FCALL_BY_NAME: {
          if (op->op2_type == IS_CONST) {
            // in ZEND_INIT_FCALL_BY_NAME_SPEC_CONST_HANDLER:
            //   function_name = (zval*)(opline->op2.zv+1); // why +1 ???
            //   zend_hash_find(EG(function_table), Z_STR_P(function_name))
            zend_string *callee = Z_STR_P(RT_CONSTANT(op_array, op->op2));

            zval *func = zend_hash_find(executor_globals.function_table, callee);
            // This is matching user-defined functions...? Needs to be builtins only.
            if (func != NULL && Z_TYPE_P(func) == IS_PTR &&
                func->value.func->type == ZEND_INTERNAL_FUNCTION) {
              ignore_call |= !IS_OPCODE_DUMP_ENABLED();
              sprintf(routine_name, "builtin:%s", callee->val);
              push_fcall_init(cfm.app, i, op->opcode, BUILTIN_ROUTINE_HASH_PLACEHOLDER, routine_name);
              break; // ignore builtins for now (unless dumping ops)
            }

            classname = COMPILED_ROUTINE_DEFAULT_SCOPE;
            sprintf(routine_name, "%s:%s", classname, callee->val);
          } else if (op->op2_type != IS_UNUSED) { // some kind of var
            uint var_index = (uint) (op->op2.var / sizeof(zval *));
            sprintf(routine_name, "<var #%d>", var_index);
          }
          to_routine_hash = hash_routine(routine_name);
          push_fcall_init(cfm.app, i, op->opcode, to_routine_hash, routine_name);
        } break;
        case ZEND_INIT_METHOD_CALL: {
          if (op->op2_type == IS_CONST && op->op1_type == IS_UNUSED)
            classname = op_array->scope->name->val;
          else
            classname = "<class-instance>";

          if (op->op2_type == IS_CONST)
            sprintf(routine_name, "%s:%s", classname, Z_STRVAL_P(RT_CONSTANT(op_array, op->op2)));
          else
            sprintf(routine_name, "%s:<var>", classname);
          to_routine_hash = hash_routine(routine_name);
          push_fcall_init(cfm.app, i, op->opcode, to_routine_hash, routine_name);
        } break;
        case ZEND_INIT_STATIC_METHOD_CALL: {
          classname = NULL;
          if (op->op1_type == IS_CONST && op->op2_type == IS_CONST) {
            classname = Z_STRVAL_P(RT_CONSTANT(op_array, op->op1));
          } else if (op->op1_type == IS_VAR && op->op2_type == IS_UNUSED) {
            if ((op-1)->extended_value == ZEND_FETCH_CLASS_SELF)
              classname = op_array->scope->name->val;
          }

          if (classname != NULL) {
            sprintf(routine_name, "%s:%s", classname, Z_STRVAL_P(RT_CONSTANT(op_array, op->op2)));
            to_routine_hash = hash_routine(routine_name);
            push_fcall_init(cfm.app, i, op->opcode, to_routine_hash, routine_name);
          }
        } break;
        case ZEND_INIT_USER_CALL: {
          //ERROR("Found ZEND_INIT_USER_CALL: check semantics!\n");
        } break;
      }

      switch (op->opcode) {
          case ZEND_NEW:
          case ZEND_INIT_FCALL:
          case ZEND_INIT_FCALL_BY_NAME:
          case ZEND_INIT_NS_FCALL_BY_NAME:
          case ZEND_INIT_METHOD_CALL:
          case ZEND_INIT_STATIC_METHOD_CALL:
          case ZEND_INIT_USER_CALL: {
            if (!has_fcall_init(i))
              push_fcall_skip(i, ignore_call ? 0 : op->opcode);
          } break;
      }
    }

    routine_cfg_assign_opcode(cfm.cfg, op->opcode, op->extended_value, op->lineno, i, USER_LEVEL_TOP);
    target = get_compiled_edge_target(op, i);
    if (target.type == COMPILED_EDGE_DIRECT) {
      if (target.index >= op_array->last) {
        ERROR("Skipping foobar edge %u|0x%x(%u,%u) -> %u in {%s|%s, 0x%x}\n", // alpha: getting a few of these
              i, op->opcode, op->op1_type, op->op2_type, target.index,
              fqn->unit.path, fqn->function.cfm.routine_name,
              fqn->function.callee_hash);
        continue;
      }

#ifdef SPOT_DEBUG
      if (spot) {
        SPOT("Compiling opcode edge %d -> %d for opcode 0x%x in %s (0x%x)\n",
             i, target.index, op->opcode, routine_name, fqn->function.callee_hash);
      }
#endif

      routine_cfg_add_opcode_edge(cfm.cfg, i, target.index, USER_LEVEL_TOP);
      PRINT("\t[create edge %u|0x%x(%u,%u) -> %u for {%s|%s, 0x%x}]\n",
            i, op->opcode, op->op1_type, op->op2_type, target.index,
            fqn->unit.path, fqn->function.cfm.routine_name,
            fqn->function.callee_hash);
    }
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
              fqn->function.callee_hash);
      }
#endif
      write_node(cfm.app, fqn->function.callee_hash, cfg_opcode, i);
    }
    for (i = 0; i < cfm.cfg->opcode_edges.size; i++) {
      cfg_edge = routine_cfg_get_opcode_edge(cfm.cfg, i);
      write_op_edge(cfm.app, fqn->function.callee_hash, cfg_edge->from_index,
                    cfg_edge->to_index, USER_LEVEL_TOP);
#ifdef SPOT_DEBUG
      if (spot) {
        SPOT("\t[emit %d -> %d in 0x%x]\n", cfg_edge->from_index,
             cfg_edge->to_index, fqn->function.callee_hash);
      }
#endif
    }
    WARN("No dataset for routine 0x%x\n", fqn->function.callee_hash);
  } else if (is_eval_routine(fqn->function.callee_hash)) {
    cfg_opcode_edge_t *cfg_edge;
    for (i = 0; i < cfm.cfg->opcodes.size; i++) {
      dataset_routine_verify_opcode(cfm.dataset, i,
                                    routine_cfg_get_opcode(cfm.cfg, i)->opcode);
    }
    for (i = 0; i < cfm.cfg->opcode_edges.size; i++) {
      cfg_edge = routine_cfg_get_opcode_edge(cfm.cfg, i);
      dataset_routine_verify_compiled_edge(cfm.dataset, cfg_edge->from_index, cfg_edge->to_index);
    }
    WARN("Found dataset for routine 0x%x\n", fqn->function.callee_hash);
  }

  flush_all_outputs(cfm.app);
  return fqn;
}

void function_created(zend_op_array *src, zend_op_array *f)
{
  if (f == NULL)
    return;

  function_fqn_t *fqn = sctable_lookup(&routines_by_opcode_address, hash_addr(f->opcodes));
  if (fqn != NULL)
    return;
  if (src != NULL)
    fqn = sctable_lookup(&routines_by_opcode_address, hash_addr(src->opcodes));
  if (fqn == NULL)
    fqn = register_new_function(f);

  sctable_add_or_replace(&routines_by_opcode_address, hash_addr(f->opcodes), fqn);

  PRINT("Registered %s with opcodes "PX"\n", fqn->function.cfm.routine_name, p2int(f->opcodes));
}

// (can never find an eval this way)
control_flow_metadata_t *get_cfm_by_name(const char *routine_name)
{
  function_fqn_t *fqn;

  fqn = (function_fqn_t *) sctable_lookup(&routines_by_callee_hash, hash_routine(routine_name));
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

control_flow_metadata_t *get_last_eval_cfm()
{
  return &last_eval_cfm;
}

compiled_edge_target_t get_compiled_edge_target(zend_op *op, uint op_index)
{
  compiled_edge_target_t target = { COMPILED_EDGE_NONE, COMPILED_EDGE_UNKNOWN_TARGET };

  switch (op->opcode) {
    case ZEND_JMP:
      target.type = COMPILED_EDGE_DIRECT;
      target.index = op_index + (OP_JMP_ADDR(op, op->op1) - op);
      break;
    case ZEND_JMPZ:
    case ZEND_JMPNZ:
    case ZEND_JMPZNZ:
    case ZEND_JMPZ_EX:
    case ZEND_JMPNZ_EX:
    case ZEND_FE_RESET_R:
    case ZEND_FE_FETCH_R:
      target.type = COMPILED_EDGE_DIRECT;
      target.index = op_index + (OP_JMP_ADDR(op, op->op2) - op);
      break;
    case ZEND_DO_FCALL:
    case ZEND_INCLUDE_OR_EVAL:
      target.type = COMPILED_EDGE_CALL;
      break;
    case ZEND_THROW:
    case ZEND_NEW:
      target.type = COMPILED_EDGE_DIRECT;
      target.index = op_index + 2;
      break;
  }

  return target;
}
