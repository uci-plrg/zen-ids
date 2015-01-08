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
    PRINT("Error: function_name exceeds max length 256!\n"); \
} while (0)

static cfg_node_t last_executed_node = { ZEND_NOP, 0 };

static uint pending_cfm_frame;
static control_flow_metadata_t *call_to_eval = (control_flow_metadata_t *)int2p(1);
static control_flow_metadata_t *initial_context = (control_flow_metadata_t *)int2p(2);

static control_flow_metadata_t *pending_cfm_stack[MAX_STACK_FRAME];

static routine_cfg_t *live_loader_cfg;
static char last_unknown_function_name[MAX_FUNCTION_NAME];
static char pending_load_function_name[MAX_FUNCTION_NAME];

static inline void pend_cfm(control_flow_metadata_t *cfm)
{
  pending_cfm_stack[pending_cfm_frame++] = cfm;
  if (pending_cfm_frame >= MAX_STACK_FRAME)
    PRINT("Error: pending_cfm_frame exceeds max stack frame!\n");
}

static inline control_flow_metadata_t *pop_cfm()
{
  pending_cfm_frame--;
  if (pending_cfm_frame == 0)
    PRINT("Error: pending_cfm_frame hit stack bottom!\n");
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
      PRINT("Error! Operand type 0x%x refers to null string!\n", op->op2_type);
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
  
  pending_cfm = get_cfm(function_name);
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

static void opcode_executing(const zend_op *op)
{
  zend_op *current_opcodes;
  uint hash;
  cfg_node_t node = { op->opcode, 0 };
  routine_cfg_t *current_cfg = get_current_interp_routine_cfg();
  control_flow_metadata_t *pending_cfm;
  
  if (EG(current_execute_data) != NULL && EG(current_execute_data)->func != NULL)
    current_opcodes = EG(current_execute_data)->func->op_array.opcodes;

  if (current_opcodes == NULL) {
    PRINT("Error: opcode array not found for execution of opcode 0x%x\n", op->opcode);
    node.index = 0xffffffff;
  } else {
    node.index = (uint)(op - current_opcodes);
  }
  
  if (current_cfg == NULL) {
    PRINT("  === executing %s at index %d of %s on line %d with ops "PX"\n", 
          zend_get_opcode_name(node.opcode), 
          node.index, get_current_interp_context_name(), op->lineno,
          p2int(current_opcodes));
  } else {
    PRINT("  === executing %s at index %d of %s(0x%x|0x%x) on line %d with ops "PX"\n", 
          zend_get_opcode_name(node.opcode), node.index, 
          get_current_interp_context_name(), current_cfg->unit_hash, 
          current_cfg->routine_hash, op->lineno, p2int(current_opcodes));
  }
  //PRINT("[%s(0x%x):%d, line %d]: 0x%x:%s\n", get_current_interp_context_name(), get_current_interp_context_id(), 
  //  node.index, op->lineno, op->opcode, zend_get_opcode_name(op->opcode));
  
  verify_interp_context(current_opcodes, node);
  
  last_executed_node = node;

  switch (op->opcode) {
    case ZEND_INCLUDE_OR_EVAL:
      push_interp_context(current_opcodes, node.index, null_cfm);
      //pend_cfm(call_to_eval);
      break;
    case ZEND_INIT_FCALL:
    case ZEND_INIT_METHOD_CALL:
    case ZEND_INIT_STATIC_METHOD_CALL:
    case ZEND_INIT_FCALL_BY_NAME:
      PRINT("  === ZEND_INIT_FCALL | ZEND_INIT_METHOD_CALL | ZEND_INIT_STATIC_METHOD_CALL | ZEND_INIT_FCALL_BY_NAME\n");
      init_call(op);
      //if (op->opcode != ZEND_INIT_STATIC_METHOD_CALL)
        break;
    case ZEND_DO_FCALL: 
      pending_cfm = pop_cfm();
      if (pending_cfm != NULL) {
        if (pending_cfm == call_to_eval) {
          push_interp_context(current_opcodes, node.index, null_cfm);
        } else {
          push_interp_context(current_opcodes, node.index, *pending_cfm);
        }
      }
      break;
    case ZEND_RETURN: {
      bool is_loader_frame = (get_current_interp_routine_cfg() == live_loader_cfg);
      PRINT("  === return\n");
      pop_interp_context();
      
      if (is_loader_frame) {
        live_loader_cfg = NULL;
        if (peek_cfm() == NULL) {
          PRINT("Pending the function for which load was invoked: %s\n", pending_load_function_name);
          push_interp_context(current_opcodes, 0xffffffffU, *get_cfm(pending_load_function_name));
          //set_interp_cfm(*get_cfm(pending_load_function_name));
        }
      }
      /*
      if (is_loader_frame) {
        PRINT("Return is from the loader frame: now pending the loaded function: %s\n",
              pending_load_function_name);
        pend_cfm(get_cfm(pending_load_function_name));
      }
      */
    } break;
    case ZEND_DECLARE_FUNCTION:
      PRINT("  === declare function\n");
      break;
    case ZEND_DECLARE_LAMBDA_FUNCTION:
      PRINT("  === ZEND_DECLARE_LAMBDA_FUNCTION\n");
      break;
    case ZEND_INIT_NS_FCALL_BY_NAME:
      PRINT("  === ZEND_INIT_NS_FCALL_BY_NAME\n");
      break;
    case ZEND_EXT_FCALL_BEGIN:
      PRINT("  === ZEND_EXT_FCALL_BEGIN\n");
      break;
    case ZEND_EXT_FCALL_END:
      PRINT("  === ZEND_EXT_FCALL_END\n");
      break;
  }
}

static void opcode_compiling(const zend_op *op, uint index)
{
  add_compiled_op(op, index);
}

static void edge_compiling(uint from_index, uint to_index)
{
  add_compiled_edge(from_index, to_index);
}

static void file_compiling(const char *path)
{
  if (path == NULL) {
    uint eval_id = get_next_eval_id();
    
    PRINT("  === entering `eval` context #%u\n", eval_id); 
    push_eval(eval_id);
  } else {
    push_compilation_unit(path);
  }
}

static void file_compiled()
{
  control_flow_metadata_t compiled_cfm = pop_compilation_unit();
  
  if (compiled_cfm.cfg == NULL) {
    PRINT("Not activating compiled unit because it was skipped by the opmon compiler.\n");
    return;
  }
  
  set_interp_cfm(compiled_cfm);
}

static void function_compiling(const char *classname, const char *function_name)
{
  push_compilation_function(classname, function_name);
}

static void function_compiled()
{
  pop_compilation_function();
}

static void routine_starting()
{
  zend_op *current_opcodes = EG(current_execute_data)->func->op_array.opcodes;
  control_flow_metadata_t *pending_cfm = pop_cfm();
  
  if (pending_cfm == initial_context)
    return;
  
  if (pending_cfm == NULL) {
    PRINT("Error: unknown routine starting with ops at "PX".\n", 
          p2int(current_opcodes));
    //strcpy(pending_load_function_name, last_unknown_function_name);
    //pending_cfm = get_cfm("<default>:{closure}");
  //} else if (pending_cfm == loader_cfm) {
  //  push_interp_context(current_opcodes, last_executed_node.index, loader_cfm);
  } else if (pending_cfm == call_to_eval) {
    PRINT("Starting eval routine\n");
    push_interp_context(current_opcodes, last_executed_node.index, null_cfm);
  } else {
    PRINT("Starting pending routine\n");
    push_interp_context(current_opcodes, last_executed_node.index, *pending_cfm);
  }
}

static void loader_starting()
{
  control_flow_metadata_t *loader_cfm = get_cfm("<default>:{closure}");
  
  PRINT("Loading new class\n");
  strcpy(pending_load_function_name, last_unknown_function_name);
  pend_cfm(loader_cfm);
  live_loader_cfg = loader_cfm->cfg;
}

void init_event_handler(zend_opcode_monitor_t *monitor)
{
  //scarray_unit_test();
  
  pending_cfm_stack[0] = NULL;
  pending_cfm_stack[1] = NULL;
  pending_cfm_frame = 1;
  pend_cfm(initial_context);
  live_loader_cfg = NULL;
  
  init_compile_context();
  init_cfg_handler();
  init_metadata_handler();
  
  monitor->set_top_level_script = starting_script;
  monitor->notify_opcode_interp = opcode_executing;
  monitor->notify_opcode_compile = opcode_compiling;
  monitor->notify_edge_compile = edge_compiling;
  monitor->notify_file_compile_start = file_compiling;
  monitor->notify_file_compile_complete = file_compiled;
  monitor->notify_function_compile_start = function_compiling;
  monitor->notify_function_compile_complete = function_compiled;
  monitor->notify_routine_execute_start = routine_starting;
  monitor->notify_class_load = loader_starting;
}

void destroy_event_handler()
{
  destroy_cfg_handler();
  destroy_metadata_handler();
}
