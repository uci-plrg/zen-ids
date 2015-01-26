#include "php_opcode_monitor.h"
#include "interp_context.h"
#include "compile_context.h"
#include "cfg_handler.h"
#include "cfg.h"
#include "lib/script_cfi_utils.h"
#include "lib/script_cfi_array.h"
#include "event_handler.h"

#define MAX_FUNCTION_NAME 256
#define MAX_STACK_FRAME 256

#define CHECK_FUNCTION_NAME_LENGTH(len) \
do { \
  if ((len) >= MAX_FUNCTION_NAME) \
    ERROR("function_name exceeds max length 256!\n"); \
} while (0)

typedef struct _pending_load_t {
  bool pending_execution;
  char function_name[MAX_FUNCTION_NAME];
} pending_load_t;

static cfg_node_t last_executed_node = { ZEND_NOP, 0 };
static zend_op *current_op_array = NULL;
static zend_op *last_op_array = NULL;

static uint pending_cfm_frame;
static control_flow_metadata_t *call_to_eval = (control_flow_metadata_t *)int2p(1);
static control_flow_metadata_t *initial_context = (control_flow_metadata_t *)int2p(2);

static control_flow_metadata_t *pending_cfm_stack[MAX_STACK_FRAME];

static routine_cfg_t *live_loader_cfg;
static char last_unknown_function_name[MAX_FUNCTION_NAME];
static pending_load_t pending_load;

cfg_t *app_cfg;

static inline void pend_cfm(control_flow_metadata_t *cfm)
{
  pending_cfm_stack[pending_cfm_frame++] = cfm;
  if (pending_cfm_frame >= MAX_STACK_FRAME)
    ERROR("pending_cfm_frame exceeds max stack frame!\n");
}

static inline control_flow_metadata_t *pop_cfm()
{
  pending_cfm_frame--;
  if (pending_cfm_frame == 0)
    ERROR("pending_cfm_frame hit stack bottom!\n");
  return pending_cfm_stack[pending_cfm_frame];
}

static inline control_flow_metadata_t *peek_cfm()
{
  return pending_cfm_stack[pending_cfm_frame-1];
}

static void init_call(const zend_op *op)
{
  uint i, j, original_function_length;
  char function_name[MAX_FUNCTION_NAME];
  control_flow_metadata_t *pending_cfm;
  
  if (op->opcode == ZEND_INIT_METHOD_CALL || op->opcode == ZEND_INIT_STATIC_METHOD_CALL) {
    zend_execute_data *execute_data = EG(current_execute_data); // referenced implicitly by EX_VAR (next line)
    const char *type = NULL;
    switch (op->op1_type) {
      case IS_CONST:
        type = op->op1.zv->value.str->val;
        break;
      case IS_CV:
      case IS_VAR:
        if (op->opcode == ZEND_INIT_METHOD_CALL)
          type = EX_VAR(op->op1.var)->value.obj->ce->name->val;
        else
          type = EX_VAR(op->op1.var)->value.ce->name->val;
        break;
      case IS_UNUSED:
        type = execute_data->This.value.obj->ce->name->val;
        break;
      default:
        PRINT("Warning: Method call to object with unknown reference\n");
        type = "<default>";
    }
    strcpy(function_name, type);
    strcat(function_name, ":");
  } else {
    strcpy(function_name, "<default>:");
  }
  j = strlen(function_name);
  CHECK_FUNCTION_NAME_LENGTH(j);
    
  if (op->op2_type == IS_CONST) {
    const char *original_function_name = op->op2.zv->value.str->val;
    original_function_length = strlen(original_function_name + 1);
    CHECK_FUNCTION_NAME_LENGTH(j + original_function_length);
    for (i = 0; i <= original_function_length; i++) {
      function_name[i+j] = tolower(original_function_name[i]);
    }      
    function_name[i+j] = '\0';
  } else if (op->op2_type == IS_CV || op->op2_type == IS_VAR) {
    zend_execute_data *execute_data = EG(current_execute_data); // referenced implicitly by EX_VAR (next line)
    zend_string *variable_name = EX_VAR(op->op2.var)->value.str;
    if (variable_name == NULL) {
      ERROR("Operand type 0x%x refers to null string!\n", op->op2_type);
      pend_cfm(NULL);
      return;
    } else {
      const char *variable = variable_name->val;
      
      if (*variable == '\0' && strncmp(variable+1, "lambda_", 7) == 0) {
        CHECK_FUNCTION_NAME_LENGTH(j + strlen(variable+1));
        strcat(function_name, variable+1);
      } else {
        original_function_length = strlen(variable);
        CHECK_FUNCTION_NAME_LENGTH(j + original_function_length + 1);
        for (i = 0; i <= original_function_length; i++) {
          function_name[i+j] = tolower(variable[i]);
        }      
        function_name[i+j] = '\0';
      }
    }
  } else {
    pend_cfm(NULL);
    PRINT("  === init call to function identified by unknown reference\n");
    return;
  }
  
  pending_cfm = get_cfm_by_name(function_name);
  if (pending_cfm == NULL) {
    if (strcmp("<default>:create_function", function_name) == 0) {
      PRINT("  === init call to create_function\n");
      pending_cfm = call_to_eval;
    } else {
      PRINT("  === init call to builtin function %s\n", function_name);
      strcpy(last_unknown_function_name, function_name);
    }
  } else {
    const char *source_path = get_function_declaration_path(function_name);
    PRINT("  === init call to function %s|%s\n", source_path, function_name);
  }
  pend_cfm(pending_cfm);
}

void init_event_handler(zend_opcode_monitor_t *monitor)
{
  //scarray_unit_test();
  
  app_cfg = cfg_new();
  
  pending_cfm_stack[0] = NULL;
  pending_cfm_stack[1] = NULL;
  pending_cfm_frame = 1;
  pend_cfm(initial_context);
  live_loader_cfg = NULL;
  
  pending_load.pending_execution = false;
  
  init_compile_context();
  init_cfg_handler();
  init_metadata_handler();
  
  monitor->set_top_level_script = starting_script;
  monitor->notify_opcode_interp = opcode_executing;
  monitor->notify_function_compile_complete = function_compiled;
  //monitor->notify_server_startup = server_startup;
  monitor->notify_worker_startup = worker_startup;
  
  server_startup();
}

void destroy_event_handler()
{
  destroy_cfg_handler();
  destroy_metadata_handler();
}
