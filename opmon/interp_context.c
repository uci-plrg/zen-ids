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

#define IS_SAME_FRAME(a, b) ((a).execute_data == (b).execute_data && (a).opcodes == (b).opcodes)

typedef struct _stack_frame_t {
  zend_execute_data *execute_data;
  zend_op *opcodes;
  zend_uchar opcode;
  union {
    uint op_index;
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
  STACK_STATE_RETURNING,
  STACK_STATE_RETURNED,
  STACK_STATE_CALL
} stack_state_t;

typedef struct _stack_event_t {
  stack_state_t state;
  zend_uchar last_opcode;
  zend_execute_data *return_target;
  const char *return_target_name;
} stack_event_t;

static exception_frame_t exception_stack[MAX_STACK_FRAME_exception_stack];
static exception_frame_t *exception_frame;

static lambda_frame_t lambda_stack[MAX_STACK_FRAME_lambda_stack];
static lambda_frame_t *lambda_frame;

static sctable_t frame_table;
static stack_frame_t base_frame;
static stack_frame_t prev_frame;
static stack_frame_t cur_frame;

static stack_event_t stack_event;

static user_session_t current_session;

static uint op_execution_count = 0;

static pthread_t first_thread_id;

static zend_op entry_op;

#define CONTEXT_ENTRY 0xffffffffU

void initialize_interp_context()
{
  frame_table.hash_bits = 7;
  sctable_init(&frame_table);
  memset(&base_frame, 0, sizeof(stack_frame_t));
  memset(&prev_frame, 0, sizeof(stack_frame_t));
  memset(&cur_frame, 0, sizeof(stack_frame_t));

  // fake node for entry point
  memset(&entry_op, 0, sizeof(zend_op));
  entry_op.opcode = ENTRY_POINT_OPCODE;
  entry_op.extended_value = ENTRY_POINT_EXTENDED_VALUE;
  base_frame.opcodes = &entry_op;
  base_frame.opcode = entry_op.opcode;
  base_frame.cfm.cfg = routine_cfg_new(ENTRY_POINT_HASH);
  cur_frame = base_frame;
  routine_cfg_assign_opcode(cur_frame.cfm.cfg, ENTRY_POINT_OPCODE,
                            ENTRY_POINT_EXTENDED_VALUE, 0, 0);


  memset(exception_stack, 0, 2 * sizeof(exception_frame_t));
  exception_frame = exception_stack + 1;

  memset(lambda_stack, 0, 2 * sizeof(lambda_frame_t));
  lambda_frame = lambda_stack + 1;

  stack_event.state = STACK_STATE_NONE;
  stack_event.last_opcode = 0;

  first_thread_id = pthread_self();

  current_session.user_level = USER_LEVEL_BOTTOM;
}

void load_entry_point_dataset(application_t *app)
{
  base_frame.cfm.dataset = dataset_routine_lookup(app, ENTRY_POINT_HASH);
  if (base_frame.cfm.dataset == NULL)
    write_node(app, ENTRY_POINT_HASH, routine_cfg_get_opcode(base_frame.cfm.cfg, 0), 0);
}

static void push_exception_frame()
{
  INCREMENT_STACK(exception_stack, exception_frame);
  *exception_frame = cur_frame;
}

static void generate_routine_edge(control_flow_metadata_t *from_cfm, uint from_index,
                                  routine_cfg_t *to_cfg, uint to_index)
{
  bool write_edge = true;
  cfg_add_routine_edge(from_cfm->cfg, from_index, to_cfg, to_index, current_session.user_level);

  if (from_cfm->dataset != NULL) {
    if (dataset_verify_routine_edge(from_cfm->app, from_cfm->dataset, from_index, to_index,
                                    to_cfg->routine_hash, current_session.user_level)) {
      write_edge = false;
      PRINT("<MON> Verified routine edge [0x%x|%u -> 0x%x]\n",
            from_cfm->cfg->routine_hash, from_index, to_cfg->routine_hash);
    }
  }

  if (write_edge) {
    zend_uchar opcode = routine_cfg_get_opcode(from_cfm->cfg, from_index)->opcode;
    WARN("<MON> New routine edge from op 0x%x [0x%x %u -> 0x%x]\n",
          opcode, from_cfm->cfg->routine_hash, from_index, to_cfg->routine_hash);
    write_routine_edge(from_cfm->app, from_cfm->cfg->routine_hash, from_index,
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
    write_op_edge(cfm->app, cfm->cfg->routine_hash, from_index, to_index, current_session.user_level);
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

static void lookup_cfm_by_name(zend_execute_data *execute_data, zend_op_array *op_array,
                               control_flow_metadata_t *cfm)
{
  char routine_name[ROUTINE_NAME_LENGTH];
  const char *classname, *function_name;
  control_flow_metadata_t *monitored_cfm;

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
    cfm->routine_name = (const char *)routine_name_buffer;
    cfm->cfg = NULL;
    cfm->dataset = NULL;
    ERROR("Failed to find opcodes for function %s\n", routine_name);
  } else {
    *cfm = *monitored_cfm;
  }
}

static void lookup_cfm(zend_execute_data *execute_data, zend_op_array *op_array,
                       control_flow_metadata_t *cfm)
{
  control_flow_metadata_t *monitored_cfm = get_cfm_by_opcodes_address(op_array->opcodes);
  if (monitored_cfm == NULL)
    lookup_cfm_by_name(execute_data, op_array, cfm);
  else
    *cfm = *monitored_cfm;
}

static inline zend_execute_data *find_return_target(zend_execute_data *execute_data)
{
  zend_execute_data *return_target = execute_data->prev_execute_data;
  while (return_target != NULL &&
         (return_target->func == NULL ||
          return_target->func->op_array.type == ZEND_INTERNAL_FUNCTION))
    return_target = return_target->prev_execute_data;
  return return_target;
}

static bool validate_return(zend_execute_data *execute_data)
{
  zend_execute_data *return_target = find_return_target(execute_data);
  return (stack_event.return_target == execute_data || stack_event.return_target == return_target);
}

static bool update_stack_frame(const zend_op *op) // true if the stack pointer changed
{
  zend_execute_data *execute_data = EG(current_execute_data), *prev_execute_data;
  zend_op_array *op_array = &execute_data->func->op_array;
  stack_frame_t new_cur_frame, new_prev_frame;

  if (op_array == NULL || op_array->opcodes == NULL)
    return false; // nothing to do

  //if (stack_event.state == STACK_STATE_UNWINDING)
  //  return false;

  if (execute_data == cur_frame.execute_data &&
      op_array->opcodes == cur_frame.opcodes) {
    prev_frame = cur_frame;
    cur_frame.op_index = (execute_data->opline - op_array->opcodes);
    cur_frame.opcode = op->opcode;
    switch (stack_event.state) {
      case STACK_STATE_CALL:
        ERROR("Stack frame did not change after STACK_STATE_CALL at opcode 0x%x. Still in %s\n",
              stack_event.last_opcode, cur_frame.cfm.routine_name);
        stack_event.state = STACK_STATE_NONE;
        break;
      case STACK_STATE_RETURNING:
        if (!validate_return(execute_data))
          ERROR("Failed to clear STACK_STATE_RETURNING. Now in %s\n", cur_frame.cfm.routine_name);
      case STACK_STATE_RETURNED:
        stack_event.state = STACK_STATE_NONE;
        break;
      default: ;
    }
    return false; // nothing to do
  }

  new_cur_frame.execute_data = execute_data;
  new_cur_frame.opcodes = op_array->opcodes;
  new_cur_frame.opcode = op->opcode;
  new_cur_frame.op_index = (execute_data->opline - op_array->opcodes);
  if (op_array->type == ZEND_EVAL_CODE) { // eval or lambda
    new_cur_frame.cfm = get_last_eval_cfm();
  } else {
    lookup_cfm(execute_data, op_array, &new_cur_frame.cfm);
  }

  switch (stack_event.state) {
    case STACK_STATE_CALL:
      stack_event.state = STACK_STATE_NONE;
      break;
    case STACK_STATE_RETURNING:
      if (validate_return(execute_data)) {
        stack_event.state = STACK_STATE_RETURNED;
      } else {
        ERROR("Expected return to %s but returned to %s\n", stack_event.return_target_name,
              new_cur_frame.cfm.routine_name);
      }
      break;
    default: ;
  }

  prev_execute_data = execute_data->prev_execute_data;
  while (prev_execute_data != NULL &&
         (prev_execute_data->func == NULL ||
          prev_execute_data->func->op_array.type == ZEND_INTERNAL_FUNCTION))
    prev_execute_data = prev_execute_data->prev_execute_data;
  if (prev_execute_data == NULL) {
    new_prev_frame = base_frame;
  } else {
    zend_op_array *prev_op_array = &prev_execute_data->func->op_array;
    new_prev_frame.execute_data = prev_execute_data;
    new_prev_frame.opcodes = prev_op_array->opcodes;
    new_prev_frame.opcode = prev_execute_data->opline->opcode;
    new_prev_frame.op_index = (prev_execute_data->opline - prev_op_array->opcodes);
    lookup_cfm(prev_execute_data, prev_op_array, &new_prev_frame.cfm);
  }

  if (stack_event.state == STACK_STATE_RETURNED) {
    PRINT("<0x%x> Routine return from %s to %s with opcodes at "PX"|"PX" and cfg "PX".\n",
          getpid(), cur_frame.cfm.routine_name, new_cur_frame.cfm.routine_name, p2int(execute_data),
          p2int(op_array->opcodes), p2int(cur_frame.cfm.cfg));
  } else {
    // TODO: skip builtins like
    //   op->op2.zv != NULL &&
    //   zend_hash_find(executor_globals.function_table, Z_STR_P(op->op2.zv)) == NULL
    if (new_cur_frame.cfm.cfg != NULL && execute_data->prev_execute_data != NULL) {
      zend_op *new_prev_op = &new_prev_frame.opcodes[new_prev_frame.op_index];
      compiled_edge_target_t compiled_target = get_compiled_edge_target(new_prev_op,
                                                                        new_prev_frame.op_index);

      if (!cfg_has_routine_edge(new_prev_frame.cfm.cfg, new_prev_frame.op_index,
                                new_cur_frame.cfm.cfg, 0)) {
        if (compiled_target.type != COMPILED_EDGE_CALL && new_prev_op->opcode != ZEND_NEW)
          WARN("Generating call edge for compiled target type %d (opcode 0x%x)\n",
               compiled_target.type, new_prev_op->opcode);
        if (IS_SAME_FRAME(new_prev_frame, base_frame)) {
          SPOT("Entry edge to %s (0x%x)\n", new_cur_frame.cfm.routine_name,
               new_cur_frame.cfm.cfg->routine_hash);
        }

        generate_routine_edge(&new_prev_frame.cfm, new_prev_frame.op_index,
                              new_cur_frame.cfm.cfg, 0); // 0 means entry, even if that opcode is not executable
      } else {
        PRINT("(skipping existing routine edge)\n");
      }

      PRINT("<0x%x> Routine call from %s to %s with opcodes at "PX"|"PX" and cfg "PX"\n",
            getpid(), new_prev_frame.cfm.routine_name, new_cur_frame.cfm.routine_name,
            p2int(execute_data), p2int(op_array->opcodes), p2int(new_cur_frame.cfm.cfg));
    }
  }

  cur_frame = new_cur_frame;
  prev_frame = new_prev_frame;
  return true;
}

static void update_user_session()
{
  if (is_php_session_active()) {
    zend_string *key = zend_string_init(USER_SESSION_KEY, sizeof(USER_SESSION_KEY) - 1, 0);
    zval *session_zval = php_get_session_var(key);
    if (session_zval == NULL || Z_TYPE_INFO_P(session_zval) != IS_LONG) {
      set_opmon_user_level(USER_LEVEL_BOTTOM);
      PRINT("<session> Session has no user level for key %s during update on pid 0x%x"
            "--assigning bottom level\n", key->val, getpid());
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
  uint i = from_index + 1;
  while (true) {
    opcode = cur_frame.opcodes[i].opcode;
    if (zend_get_opcode_name(opcode) != NULL)
      break;
    i++;
  }
  return i;
}

static bool is_fallthrough()
{
  uint from_index;
  if (prev_frame.execute_data == NULL)
    return true;

  switch (prev_frame.opcodes[prev_frame.op_index].opcode) {
    case ZEND_RETURN:
    case ZEND_JMP:
    case ZEND_BRK:
    case ZEND_CONT:
      return false;
  }
  from_index = (IS_SAME_FRAME(cur_frame, prev_frame) ? prev_frame.op_index  : 0);
  return cur_frame.op_index == get_next_executable_index(from_index);
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
    flush_all_outputs(cur_frame.cfm.app);

  if (pthread_self() != first_thread_id) {
    ERROR("Multiple threads are not supported (started on 0x%x, current is 0x%x)\n",
          (uint) first_thread_id, (uint) pthread_self());
    exit(1);
  }

  if (op->opcode == ZEND_HANDLE_EXCEPTION) {
    PRINT("@ Processing ZEND_HANDLE_EXCEPTION in stack state %u of "PX"|"PX"\n",
          stack_event.state, p2int(execute_data), p2int(op_array->opcodes));
    if (stack_event.state == STACK_STATE_NONE) {
      //zend_op *throw_op = &cur_frame.opcodes[cur_frame.last_index];
      stack_event.state = STACK_STATE_UNWINDING;
      push_exception_frame();
      WARN("Exception thrown at op %d in opcodes "PX"|"PX" of routine 0x%x\n",
           cur_frame.op_index, p2int(execute_data), p2int(op_array->opcodes),
           cur_frame.cfm.cfg->routine_hash);
    }
    return;
  }

  stack_pointer_moved = update_stack_frame(op);

  if (cur_frame.cfm.cfg->routine_hash == 0x35b71951)
    SPOT("\twp-admin/admin.php: %d\n", op->lineno);

  switch (op->opcode) {
    case ZEND_DO_FCALL:
      if (EX(call)->func->type != ZEND_INTERNAL_FUNCTION) {
        stack_event.state = STACK_STATE_CALL;
        stack_event.last_opcode = op->opcode;
      }
      break;
    case ZEND_INCLUDE_OR_EVAL:
      if (op->extended_value == ZEND_INCLUDE || op->extended_value == ZEND_REQUIRE) {
        stack_event.state = STACK_STATE_CALL;
        stack_event.last_opcode = op->opcode;
      }
      break;
    /*
    case ZEND_FETCH_OBJ_R:
    case ZEND_FETCH_OBJ_W:
    case ZEND_FETCH_OBJ_RW:
    case ZEND_FETCH_OBJ_IS:
    case ZEND_FETCH_OBJ_UNSET:
    case ZEND_ISSET_ISEMPTY_DIM_OBJ:
    case ZEND_ASSIGN_OBJ:
    case ZEND_ISSET_ISEMPTY_PROP_OBJ:
    */
    case ZEND_RETURN: {
      control_flow_metadata_t return_target_cfm;
      zend_execute_data *return_target = find_return_target(execute_data);
      if (return_target == NULL) {
        stack_event.return_target = NULL;
        stack_event.return_target_name = "<base-frame>";
      } else {
        lookup_cfm(return_target, &return_target->func->op_array, &return_target_cfm);
        stack_event.return_target = return_target;
        stack_event.return_target_name = return_target_cfm.routine_name;
      }
      PRINT("Preparing return at op %d of %s to %s\n", cur_frame.op_index, cur_frame.cfm.routine_name,
            stack_event.return_target_name);
      stack_event.state = STACK_STATE_RETURNING;
    } break;
    case ZEND_FAST_RET:
      //exception_frame->suspended = false;
      stack_event.state = STACK_STATE_UNWINDING;
      break;
    case ZEND_CATCH:
      break;
    default:
      if (stack_event.state == STACK_STATE_UNWINDING)
        stack_event.state = STACK_STATE_NONE;
      break;
  }

  if (op->opcode == ZEND_CATCH) {
    if (stack_event.state == STACK_STATE_UNWINDING) {
      WARN("Exception at op %d of 0x%x caught at op index %d in opcodes "PX"|"PX" of 0x%x\n",
           exception_frame->throw_index,
           exception_frame->cfm.cfg->routine_hash,
           cur_frame.op_index,
           p2int(cur_frame.execute_data), p2int(cur_frame.opcodes),
           cur_frame.cfm.cfg->routine_hash);
      stack_event.state = STACK_STATE_NONE;

      if (exception_frame->execute_data == cur_frame.execute_data) {
        if (!routine_cfg_has_opcode_edge(cur_frame.cfm.cfg, exception_frame->throw_index,
                                         cur_frame.op_index)) {
          generate_opcode_edge(&cur_frame.cfm, exception_frame->throw_index,
                               cur_frame.op_index);
        } else {
          PRINT("(skipping existing exception edge)\n");
        }
      } else if (!cfg_has_routine_edge(exception_frame->cfm.cfg, exception_frame->throw_index,
                                       cur_frame.cfm.cfg, cur_frame.op_index)) {
        generate_routine_edge(&exception_frame->cfm, exception_frame->throw_index,
                              cur_frame.cfm.cfg, cur_frame.op_index);
      } else {
        PRINT("(skipping existing exception edge)\n");
      }
      DECREMENT_STACK(exception_stack, exception_frame);
      caught_exception = true;
    } // else it was matching the Exception type (and missed)
  } else if (stack_event.state == STACK_STATE_UNWINDING) {
    WARN("Executing op %s while unwinding an exception!\n", zend_get_opcode_name(op->opcode));
  }

  if (cur_frame.cfm.cfg == NULL) {
    ERROR("No cfg for opcodes at "PX"|"PX"\n", p2int(execute_data),
          p2int(execute_data->func->op_array.opcodes));
  } else {
    cfg_opcode_t *expected_opcode;

    if (cur_frame.op_index >= cur_frame.cfm.cfg->opcodes.size) {
      ERROR("attempt to execute foobar op %u in opcodes "PX"|"PX" of routine 0x%x\n",
            cur_frame.op_index, p2int(execute_data), p2int(op_array->opcodes),
            cur_frame.cfm.cfg->routine_hash);
      return;
    }

    PRINT("@ Executing %s at index %u of 0x%x\n",
          zend_get_opcode_name(cur_frame.opcode), cur_frame.op_index,
          cur_frame.cfm.cfg->routine_hash);

    expected_opcode = routine_cfg_get_opcode(cur_frame.cfm.cfg, cur_frame.op_index);
    if (cur_frame.opcode != expected_opcode->opcode &&
        !is_alias(cur_frame.opcode, expected_opcode->opcode)) {
      ERROR("Expected opcode %s at index %u, but found opcode %s in opcodes "
            PX"|"PX" of routine 0x%x\n",
            zend_get_opcode_name(expected_opcode->opcode), cur_frame.op_index,
            zend_get_opcode_name(cur_frame.opcode), p2int(execute_data),
            p2int(op_array->opcodes), cur_frame.cfm.cfg->routine_hash);
    }

    // slightly weak for returns: not checking continuation pc
    if (stack_event.state == STACK_STATE_RETURNED || is_fallthrough()) {
      PRINT("@ Verified fall-through %u -> %u in 0x%x\n",
            prev_frame.op_index, cur_frame.op_index,
            cur_frame.cfm.cfg->routine_hash);
    } else if (!caught_exception && !stack_pointer_moved) {
      uint i;
      bool found = false;
      zend_op *from_op = &prev_frame.opcodes[prev_frame.op_index];

      for (i = 0; i < cur_frame.cfm.cfg->opcode_edges.size; i++) {
        cfg_opcode_edge_t *edge = routine_cfg_get_opcode_edge(cur_frame.cfm.cfg, i);
        if (edge->from_index == prev_frame.op_index) {
          if (edge->to_index == cur_frame.op_index) {
            found = true;
            break;
          }
        }
      }
      if (found) {
        PRINT("@ Verified opcode edge %u -> %u\n",
              prev_frame.op_index, cur_frame.op_index);
      } else { // slightly weak
        compiled_edge_target_t compiled_target;
        compiled_target = get_compiled_edge_target(from_op, prev_frame.op_index);
        if (compiled_target.type != COMPILED_EDGE_INDIRECT) {
          WARN("Generating indirect edge from compiled target type %d (opcode 0x%x)\n",
               compiled_target.type, op->opcode);
        }

        if (!routine_cfg_has_opcode_edge(cur_frame.cfm.cfg, prev_frame.op_index,
                                         cur_frame.op_index)) {
          generate_opcode_edge(&cur_frame.cfm, prev_frame.op_index, cur_frame.op_index);
        }
      }
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
