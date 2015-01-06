#include "php_opcode_monitor.h"
#include "interp_context.h"
#include "compile_context.h"
#include "cfg_handler.h"
#include "cfg.h"
#include "lib/script_cfi_utils.h"
#include "event_handler.h"

static control_flow_metadata_t *pending_cfm = NULL;
static control_flow_metadata_t *call_to_eval = (control_flow_metadata_t *)int2p(1);

static void init_call(const zend_op *op)
{
  uint i, j, original_function_length;
  char function_name[256];
  
  if (op->opcode == ZEND_INIT_METHOD_CALL && (op->op1_type == IS_CV || op->op1_type == IS_VAR)) {
    zend_execute_data *execute_data = EG(current_execute_data); // referenced implicitly by EX_VAR (next line)
    const char *type = EX_VAR(op->op1.var)->value.obj->ce->name->val;
    
    strcpy(function_name, type);
    strcat(function_name, ":");
  } else {
    strcpy(function_name, "<default>:");
  }
  j = strlen(function_name);
    
  if (op->op2_type == IS_CONST) {
    const char *original_function_name = op->op2.zv->value.str->val;
    original_function_length = strlen(original_function_name);
    for (i = 0; i < original_function_length; i++) {
      function_name[i+j] = tolower(original_function_name[i]);
    }      
    function_name[i+j] = '\0';
  } else if (op->op2_type == IS_CV || op->op2_type == IS_VAR) {
    zend_execute_data *execute_data = EG(current_execute_data); // referenced implicitly by EX_VAR (next line)
    const char *variable = EX_VAR(op->op2.var)->value.str->val;
    
    if (*variable == '\0' && strncmp(variable+1, "lambda_", 7) == 0) {
      strcat(function_name, variable+1);
    } else {
      original_function_length = strlen(variable);
      for (i = 0; i < original_function_length; i++) {
        function_name[i+j] = tolower(variable[i]);
      }      
      function_name[i+j] = '\0';
    }
  } else {
    pending_cfm = NULL;
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
    }
  } else {
    const char *source_path = get_function_declaration_path(function_name);
    PRINT("  === init call to function %s|%s\n", source_path, function_name);
  }
}

static void opcode_executing(const zend_op *op)
{
  zend_op *current_opcodes;
  uint hash;
  cfg_node_t node = { op->opcode, 0 };
  routine_cfg_t *current_cfg = get_current_interp_routine_cfg();
  
  if (EG(current_execute_data) != NULL && EG(current_execute_data)->func != NULL)
    current_opcodes = EG(current_execute_data)->func->op_array.opcodes;

  if (current_opcodes == NULL)
    node.index = 0xffffffff;
  else
    node.index = (uint)(op - current_opcodes);
  
  if (current_cfg == NULL) {
    PRINT("  === executing %s at index %d of %s on line %d\n", zend_get_opcode_name(node.opcode), 
          node.index, get_current_interp_context_name(), op->lineno);
  } else {
    PRINT("  === executing %s at index %d of %s(0x%x|0x%x) on line %d\n", zend_get_opcode_name(node.opcode), 
          node.index, get_current_interp_context_name(), current_cfg->unit_hash, current_cfg->routine_hash, 
          op->lineno);
  }
  //PRINT("[%s(0x%x):%d, line %d]: 0x%x:%s\n", get_current_interp_context_name(), get_current_interp_context_id(), 
  //  node.index, op->lineno, op->opcode, zend_get_opcode_name(op->opcode));
  
  verify_interp_context(current_opcodes, node);

  switch (op->opcode) {
    case ZEND_INCLUDE_OR_EVAL:
      push_interp_context(current_opcodes, node.index, null_cfm);
      break;
    case ZEND_INIT_FCALL:
    case ZEND_INIT_METHOD_CALL:
    case ZEND_INIT_FCALL_BY_NAME:
      PRINT("  === ZEND_INIT_FCALL | ZEND_INIT_METHOD_CALL | ZEND_INIT_FCALL_BY_NAME\n");
      init_call(op);
      break;
    case ZEND_DO_FCALL:
      if (pending_cfm != NULL) {
        if (pending_cfm == call_to_eval) {
          push_interp_context(current_opcodes, node.index, null_cfm);
        } else {
          push_interp_context(current_opcodes, node.index, *pending_cfm);
        }
      }
      break;
    case ZEND_RETURN:
      PRINT("  === return\n");
      pop_interp_context();
      break;
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

void init_event_handler(zend_opcode_monitor_t *monitor)
{
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
}

void destroy_event_handler()
{
  destroy_cfg_handler();
  destroy_metadata_handler();
}
