#include "php.h"
#include <pthread.h>
#include "php_opcode_monitor.h"
#include "lib/script_cfi_utils.h"
#include "lib/script_cfi_hashtable.h"
#include "metadata_handler.h"
#include "dataset.h"
#include "cfg_handler.h"
#include "compile_context.h"
#include "interp_context.h"

#define MAX_STACK_FRAME_shadow_stack 0x1000
#define MAX_STACK_FRAME_exception_stack 0x100
#define MAX_STACK_FRAME_lambda_stack 0x100
#define ROUTINE_NAME_LENGTH 256
#define FLUSH_MASK 0xff

#define IS_TOP_FRAME(ex) \
  (VM_FRAME_KIND((ex)->frame_info) == VM_FRAME_TOP_CODE || \
   VM_FRAME_KIND((ex)->frame_info) == VM_FRAME_TOP_FUNCTION)

typedef struct _stack_frame_t {
  zend_execute_data *execute_data;
  zend_op *opcodes;
  union {
    uint last_index;
    uint throw_index;
  };
  control_flow_metadata_t cfm;
} stack_frame_t;

typedef stack_frame_t exception_frame_t;

typedef struct _lambda_frame_t {
  const char *name;
} lambda_frame_t;

typedef enum _stack_state_t {
  STACK_STATE_NONE,
  STACK_STATE_UNWINDING,
  STACK_STATE_RETURNING
} stack_state_t;

typedef struct _stack_event_t {
  stack_state_t state;
} stack_event_t;

static exception_frame_t exception_stack[MAX_STACK_FRAME_exception_stack];
static exception_frame_t *exception_frame;

static lambda_frame_t lambda_stack[MAX_STACK_FRAME_lambda_stack];
static lambda_frame_t *lambda_frame;

static sctable_t frame_table;
static stack_frame_t base_frame;
static stack_frame_t *live_frame;

static stack_event_t stack_event;

static user_session_t current_session;

static uint op_execution_count = 0;

static uint first_thread_id;

static zend_op entry_op;

#define CONTEXT_ENTRY 0xffffffffU

extern cfg_files_t cfg_files;
extern cfg_t *app_cfg;

void initialize_interp_context()
{
  frame_table.hash_bits = 7;
  sctable_init(&frame_table);
  memset(&base_frame, 0, sizeof(stack_frame_t));
  live_frame = &base_frame;

  // fake node for entry point
  memset(&entry_op, 0, sizeof(zend_op));
  entry_op.opcode = ENTRY_POINT_OPCODE;
  entry_op.extended_value = ENTRY_POINT_EXTENDED_VALUE;
  live_frame->opcodes = &entry_op;
  live_frame->cfm.cfg = routine_cfg_new(ENTRY_POINT_HASH);
  routine_cfg_assign_opcode(live_frame->cfm.cfg, ENTRY_POINT_OPCODE,
                            ENTRY_POINT_EXTENDED_VALUE, 0);


  memset(exception_stack, 0, 2 * sizeof(exception_frame_t));
  exception_frame = exception_stack + 1;

  memset(lambda_stack, 0, 2 * sizeof(lambda_frame_t));
  lambda_frame = lambda_stack + 1;

  stack_event.state = STACK_STATE_NONE;

  first_thread_id = pthread_self();

  current_session.user_level = USER_LEVEL_BOTTOM;
}

void load_entry_point_dataset()
{
  base_frame.cfm.dataset = dataset_routine_lookup(ENTRY_POINT_HASH);
}

static void push_exception_frame()
{
  INCREMENT_STACK(exception_stack, exception_frame);
  *exception_frame = *live_frame;
}

static void generate_routine_edge(control_flow_metadata_t *from_cfm, uint from_index,
                                  routine_cfg_t *to_cfg, uint to_index)
{
  bool write_edge = true;
  cfg_add_routine_edge(from_cfm->cfg, from_index, to_cfg, to_index, current_session.user_level);

  if (from_cfm->dataset != NULL) {
    if (dataset_verify_routine_edge(from_cfm->dataset, from_index, to_index,
                                    to_cfg->routine_hash)) {
      write_edge = false;
      PRINT("<MON> Verified routine edge [0x%x|%u -> 0x%x]\n",
            from_cfm->cfg->routine_hash, from_index, to_cfg->routine_hash);
    }
  }

  if (write_edge) {
    zend_uchar opcode = routine_cfg_get_opcode(from_cfm->cfg, from_index)->opcode;
    WARN("<MON> New routine edge from op 0x%x [0x%x %u -> 0x%x]\n",
          opcode, from_cfm->cfg->routine_hash, from_index, to_cfg->routine_hash);
    write_routine_edge(from_cfm->cfg->routine_hash, from_index,
                       to_cfg->routine_hash, to_index, current_session.user_level);
  }
}

static void generate_opcode_edge(control_flow_metadata_t *cfm, uint from_index, uint to_index)
{
  bool write_edge = true;
  routine_cfg_add_opcode_edge(cfm->cfg, from_index, to_index, current_session.user_level);

  if (cfm->dataset != NULL) {
    if (dataset_verify_opcode_edge(cfm->dataset, from_index, to_index)) {
      write_edge = false;
      PRINT("<MON> Verified opcode edge [0x%x %u -> %u]\n",
            cfm->cfg->routine_hash, from_index, to_index);
    }
  }

  if (write_edge) {
    WARN("<MON> New opcode edge [0x%x %u -> %u]\n",
          cfm->cfg->routine_hash, from_index, to_index);
    write_op_edge(cfm->cfg->routine_hash, from_index, to_index, current_session.user_level);
  }
}

static bool is_alias(zend_uchar first_opcode, zend_uchar second_opcode)
{
  switch (first_opcode) {
    case ZEND_FETCH_IS:
    case ZEND_FETCH_FUNC_ARG:
      return second_opcode == ZEND_FETCH_R;
    case ZEND_FETCH_DIM_FUNC_ARG:
      return ZEND_FETCH_DIM_R;
  }
  return false;
}

static bool update_stack_frame(const zend_op *op) // true if the stack pointer changed
{
  control_flow_metadata_t to_cfm;
  zend_execute_data *execute_data = EG(current_execute_data);
  zend_op_array *op_array = &execute_data->func->op_array;
  stack_frame_t *activated_frame;

  if (op_array == NULL || op_array->opcodes == NULL)
    return false; // nothing to do

  //if (stack_event.state == STACK_STATE_UNWINDING)
  //  return false;

  if (execute_data == live_frame->execute_data &&
      op_array->opcodes == live_frame->opcodes) {
    if (stack_event.state == STACK_STATE_RETURNING)
      stack_event.state = STACK_STATE_NONE; // don't clear `unwinding` state
    return false; // nothing to do
  }

  activated_frame = sctable_lookup(&frame_table, hash_addr(execute_data));
  if (activated_frame != NULL) {
    sctable_remove(&frame_table, hash_addr(live_frame->execute_data));
    free(live_frame);
    live_frame = activated_frame;
    return true;
  }

  if (op_array->type == ZEND_EVAL_CODE) { // eval or lambda
    to_cfm = get_last_eval_cfm();
  } else {
    control_flow_metadata_t *monitored_cfm = NULL;

    if (monitored_cfm == NULL)
      monitored_cfm = get_cfm_by_opcodes_address(op_array->opcodes);
    if (monitored_cfm == NULL) {
      char routine_name[ROUTINE_NAME_LENGTH];
      const char *classname, *function_name;

      if (op_array->function_name == NULL) {     // script-body
        classname = (strrchr(op_array->filename->val, '/') + 1); // relativize
        function_name = "<script-body>";
      } else if (op_array->scope == NULL || (op_array->fn_flags & ZEND_ACC_CLOSURE)) {
        classname = "<default>";                 // function call
        /*
        if (strcmp(op_array->function_name->val, "__lambda_func") == 0) {
          function_name = lambda_frame->name;
          DECREMENT_STACK(lambda_stack, lambda_frame);
        } else {
        */
          function_name = op_array->function_name->val;
        //}
      } else if (execute_data->This.value.obj == NULL) { // static method call
        classname = op_array->scope->name->val;
        function_name = op_array->function_name->val;
      } else {                                   // instance method call
        classname = op_array->scope->name->val;
        function_name = op_array->function_name->val;
      }

      sprintf(routine_name, "%s:%s", classname, function_name);

      SPOT("Lookup cfm by name %s\n", routine_name);

      monitored_cfm = get_cfm_by_name(routine_name);

      if (monitored_cfm == NULL) {
        char *routine_name_buffer = malloc(strlen(routine_name)+10);
        sprintf(routine_name_buffer, "<missing>%s", routine_name);
        to_cfm.routine_name = (const char *)routine_name_buffer;
        to_cfm.cfg = NULL;
        to_cfm.dataset = NULL;
        ERROR("Failed to find opcodes for function %s\n", routine_name);
      }
    }
    if (monitored_cfm != NULL)
      to_cfm = *monitored_cfm;
  }

  // TODO: skip builtins like
  //   op->op2.zv != NULL &&
  //   zend_hash_find(executor_globals.function_table, Z_STR_P(op->op2.zv)) == NULL
  if (to_cfm.cfg != NULL) {
    zend_op *op = &live_frame->opcodes[live_frame->last_index];
    compiled_edge_target_t compiled_target = get_compiled_edge_target(op, live_frame->last_index);

    if (!cfg_has_routine_edge(live_frame->cfm.cfg, live_frame->last_index, to_cfm.cfg, 0)) {
      if (compiled_target.type != COMPILED_EDGE_CALL && op->opcode != ZEND_NEW)
        WARN("Generating call edge for compiled target type %d (opcode 0x%x)\n",
             compiled_target.type, op->opcode);
      if (live_frame == NULL)
        SPOT("Entry edge to %s (0x%x)\n", to_cfm.routine_name, to_cfm.cfg->routine_hash);

      generate_routine_edge(&live_frame->cfm, live_frame->last_index,
                            to_cfm.cfg, 0); // 0 means entry, even if that opcode is not executable
    } else {
      PRINT("(skipping existing routine edge)\n");
    }
  }

  //SPOT("<0x%x> Call %s -> %s (%d)\n", getpid(), live_frame->cfm.routine_name, to_cfm.routine_name,
  //     (int)(live_frame - shadow_stack) + 1);

  PRINT("<session> <%d|0x%x> Routine call to %s with opcodes at "PX"|"PX" and cfg "PX"\n",
        current_session.user_level, getpid(),
        to_cfm.routine_name, p2int(execute_data),
        p2int(op_array->opcodes), p2int(to_cfm.cfg));

  live_frame = malloc(sizeof(stack_frame_t));
  live_frame->execute_data = execute_data;
  live_frame->opcodes = op_array->opcodes;
  live_frame->last_index = CONTEXT_ENTRY;
  live_frame->cfm = to_cfm;
  sctable_add_or_replace(&frame_table, hash_addr(execute_data), live_frame);

  return true;
}

static void update_user_session()
{
  if (is_php_session_active()) {
    zend_string *key = zend_string_init(USER_SESSION_KEY, sizeof(USER_SESSION_KEY) - 1, 0);
    zval *session_zval = php_get_session_var(key);
    if (session_zval == NULL || Z_TYPE_INFO_P(session_zval) != IS_LONG) {
      current_session.user_level = USER_LEVEL_BOTTOM;
      PRINT("<session> Session has no user level for key %s during update on pid 0x%x"
            "--assigning level -1\n", key->val, getpid());
    } else {
      PRINT("<session> Found session user level %ld\n", Z_LVAL_P(session_zval));
      current_session.user_level = (uint) Z_LVAL_P(session_zval);
    }
    zend_string_release(key);

    PRINT("<session> Updated current user session to level %d\n", current_session.user_level);
  } else {
    // TODO: why is the session sometimes inactive??
  }
}

static uint get_next_executable_index(uint from_index)
{
  zend_uchar opcode;
  uint i = from_index;
  while (true) {
    opcode = live_frame->opcodes[i].opcode;
    if (zend_get_opcode_name(opcode) != NULL)
      break;
    i++;
  }
  return i;
}

static bool is_fallthrough(cfg_node_t *to_node)
{
  bool is_context_entry = live_frame->last_index == CONTEXT_ENTRY;
  uint next_index;

  if (!is_context_entry) {
    switch (live_frame->opcodes[live_frame->last_index].opcode) {
      case ZEND_RETURN:
      case ZEND_JMP:
      case ZEND_BRK:
      case ZEND_CONT:
        return false;
    }
  }

  next_index = (is_context_entry ? 0 : live_frame->last_index + 1);
  return to_node->index == get_next_executable_index(next_index);
}

/*
static bool is_lambda_call_init(const zend_op *op)
{
  zend_op_array *op_array = &EG(current_execute_data)->func->op_array;

  if (op->opcode != ZEND_INIT_FCALL_BY_NAME || op_array->function_name == NULL)
    return false;

  return strcmp(op_array->function_name->val, "__lambda_func") == 0;
}
*/

void opcode_executing(const zend_op *op)
{
  zend_execute_data *execute_data = EG(current_execute_data);
  zend_op_array *op_array = &execute_data->func->op_array;
  bool stack_pointer_moved, caught_exception = false;

  update_user_session();

  op_execution_count++;
  if ((op_execution_count & FLUSH_MASK) == 0)
    flush_all_outputs();

  if (pthread_self() != first_thread_id)
    ERROR("Multiple threads are not supported!\n");

  if (op->opcode == ZEND_HANDLE_EXCEPTION) {
    PRINT("@ Processing ZEND_HANDLE_EXCEPTION in stack state %u of "PX"|"PX"\n",
          stack_event.state, p2int(execute_data), p2int(op_array->opcodes));
    if (stack_event.state == STACK_STATE_NONE) {
      //zend_op *throw_op = &live_frame->opcodes[live_frame->last_index];
      stack_event.state = STACK_STATE_UNWINDING;
      push_exception_frame();
      WARN("Exception thrown at op %d in opcodes "PX"|"PX" of routine 0x%x\n",
           live_frame->last_index, p2int(execute_data), p2int(op_array->opcodes),
           live_frame->cfm.cfg->routine_hash);
    }
    return;
  }

  stack_pointer_moved = update_stack_frame(op);

  // todo: check remaining state/opcode mismatches
  if (op->opcode == ZEND_RETURN) {
    //SPOT("Preparing return from %s\n", live_frame->cfm.routine_name);
    stack_event.state = STACK_STATE_RETURNING;
  } else if (stack_event.state == STACK_STATE_UNWINDING && op->opcode != ZEND_FAST_RET &&
             op->opcode != ZEND_CATCH) {
    //exception_frame->suspended = true;
    stack_event.state = STACK_STATE_NONE;
  } else if (op->opcode == ZEND_FAST_RET) {
    //exception_frame->suspended = false;
    stack_event.state = STACK_STATE_UNWINDING;
  }

  if (op->opcode == ZEND_CATCH) {
    if (stack_event.state == STACK_STATE_UNWINDING) {
      cfg_node_t executing_node = { op->opcode, op - live_frame->opcodes };
      WARN("Exception at op %d of 0x%x caught at op %d in opcodes "PX"|"PX" of 0x%x\n",
           exception_frame->throw_index,
           exception_frame->cfm.cfg->routine_hash,
           executing_node.index,
           p2int(live_frame->execute_data), p2int(live_frame->opcodes),
           live_frame->cfm.cfg->routine_hash);
      stack_event.state = STACK_STATE_NONE;

      if (exception_frame->execute_data == live_frame->execute_data) {
        if (!routine_cfg_has_opcode_edge(live_frame->cfm.cfg, exception_frame->throw_index,
                                         executing_node.index)) {
          generate_opcode_edge(&live_frame->cfm, exception_frame->throw_index,
                               executing_node.index);
        } else {
          PRINT("(skipping existing exception edge)\n");
        }
      } else if (!cfg_has_routine_edge(exception_frame->cfm.cfg, exception_frame->throw_index,
                                       live_frame->cfm.cfg, executing_node.index)) {
        generate_routine_edge(&exception_frame->cfm, exception_frame->throw_index,
                              live_frame->cfm.cfg, executing_node.index);
      } else {
        PRINT("(skipping existing exception edge)\n");
      }
      DECREMENT_STACK(exception_stack, exception_frame);
      caught_exception = true;
    } // else it was matching the Exception type (and missed)
  } else if (stack_event.state == STACK_STATE_UNWINDING) {
    WARN("Executing op %s while unwinding an exception!\n", zend_get_opcode_name(op->opcode));
  }

  if (live_frame->cfm.cfg == NULL) {
    ERROR("No cfg for opcodes at "PX"|"PX"\n", p2int(execute_data),
          p2int(execute_data->func->op_array.opcodes));
  } else {
    cfg_node_t executing_node = { op->opcode, op - live_frame->opcodes };
    cfg_opcode_t *expected_opcode;

    if (executing_node.index >= live_frame->cfm.cfg->opcodes.size) {
      ERROR("attempt to execute foobar op %u in opcodes "PX"|"PX" of routine 0x%x\n",
            executing_node.index, p2int(execute_data), p2int(op_array->opcodes),
            live_frame->cfm.cfg->routine_hash);
    } else {
      PRINT("@ Executing %s at index %u of 0x%x\n",
            zend_get_opcode_name(executing_node.opcode), executing_node.index,
            live_frame->cfm.cfg->routine_hash);

      expected_opcode = routine_cfg_get_opcode(live_frame->cfm.cfg, executing_node.index);
      if (executing_node.opcode != expected_opcode->opcode &&
          !is_alias(executing_node.opcode, expected_opcode->opcode)) {
        ERROR("Expected opcode %s at index %u, but found opcode %s in opcodes "
              PX"|"PX" of routine 0x%x\n",
              zend_get_opcode_name(expected_opcode->opcode), executing_node.index,
              zend_get_opcode_name(executing_node.opcode), p2int(execute_data),
              p2int(op_array->opcodes), live_frame->cfm.cfg->routine_hash);
      }

      // todo: verify call continuations here too (seems to be missing)
      if (is_fallthrough(&executing_node)) {
        PRINT("@ Verified fall-through %u -> %u in 0x%x\n",
              live_frame->last_index, executing_node.index,
              live_frame->cfm.cfg->routine_hash);
      } else if (!caught_exception) {
        if (live_frame->last_index == CONTEXT_ENTRY) {
          WARN("Context entry reaches index %u, "
               "but the first executable node has index %u!\n",
               executing_node.index, get_next_executable_index(0));
        } else if (!stack_pointer_moved) {
          uint i;
          bool found = false;
          zend_op *from_op = &live_frame->opcodes[live_frame->last_index];
          cfg_node_t from_node = { op->opcode, live_frame->last_index };

          for (i = 0; i < live_frame->cfm.cfg->opcode_edges.size; i++) {
            cfg_opcode_edge_t *edge = routine_cfg_get_opcode_edge(live_frame->cfm.cfg, i);
            if (edge->from_index == live_frame->last_index) {
              if (edge->to_index == executing_node.index) {
                found = true;
                break;
              }
            }
          }
          if (found) {
            PRINT("@ Verified opcode edge %u -> %u\n",
                  live_frame->last_index, executing_node.index);
          } else if (//executing_node.index != get_next_executable_index(0) ||
              live_frame->opcodes[live_frame->last_index].opcode != ZEND_RETURN) { // slightly weak
            compiled_edge_target_t compiled_target;
            compiled_target = get_compiled_edge_target(from_op, live_frame->last_index);
            if (compiled_target.type != COMPILED_EDGE_INDIRECT) {
              WARN("Generating indirect edge from compiled target type %d (opcode 0x%x)\n",
                   compiled_target.type, op->opcode);
            }

            if (!routine_cfg_has_opcode_edge(live_frame->cfm.cfg, from_node.index,
                                             executing_node.index)) {
              generate_opcode_edge(&live_frame->cfm, from_node.index, executing_node.index);
            }
          }
        }
      }

      live_frame->last_index = executing_node.index;
    }
  }

  /*
  if (is_lambda_call_init(op)) {
    if ((op->op2_type == IS_CV || op->op2_type == IS_VAR) &&
        *EX_VAR(op->op2.var)->value.str->val == '\0') {
      INCREMENT_STACK(lambda_stack, lambda_frame);
      lambda_frame->name = EX_VAR(op->op2.var)->value.str->val + 1;
      PRINT("@ Push call to %s\n", lambda_frame->name);
    }
  }
  */
}
