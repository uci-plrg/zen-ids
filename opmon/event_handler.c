#include "php_opcode_monitor.h"
#include "interp_context.h"
#include "compile_context.h"
#include "cfg_handler.h"
#include "cfg.h"
#include "lib/script_cfi_utils.h"
#include "event_handler.h"

// todo: thread safety?
static uint eval_id = 0;

static control_flow_metadata_t *pending_cfm = NULL;

static void opcode_executing(const zend_op *op)
{
  zend_op *current_opcodes;
  uint hash;
  cfg_node_t node = { op->opcode, 0 };
  
  //if (true) return;

  if (EG(current_execute_data) != NULL && EG(current_execute_data)->func != NULL)
    current_opcodes = EG(current_execute_data)->func->op_array.opcodes;

  if (current_opcodes == NULL)
    node.index = 0xffffffff;
  else
    node.index = (uint)(op - current_opcodes);

  PRINT("  === [%s]\n", zend_get_opcode_name(op->opcode));
  //PRINT("[%s(0x%x):%d, line %d]: 0x%x:%s\n", get_current_interp_context_name(), get_current_interp_context_id(), 
  //  node.index, op->lineno, op->opcode, zend_get_opcode_name(op->opcode));
  
  verify_interp_context(current_opcodes, node);

  if (op->opcode == ZEND_INCLUDE_OR_EVAL) {
    switch (op->extended_value) {
      case ZEND_EVAL: {
        eval_id++;
        PRINT("  === entering `eval` context #%u\n", eval_id); 
      } break;
    }
    push_interp_context(current_opcodes, node.index, null_cfm);
  } else if (op->opcode == ZEND_INIT_FCALL_BY_NAME) {
    pending_cfm = get_cfm(op->op2.zv->value.str->val);
    const char *source_path = get_function_declaration_path(op->op2.zv->value.str->val);
    PRINT("  === init call to function %s|%s\n", source_path, op->op2.zv->value.str->val);
  } else if (op->opcode == ZEND_DO_FCALL) {
    push_interp_context(current_opcodes, node.index, *pending_cfm);
  } else if (op->opcode == ZEND_RETURN) {
    PRINT("  === return\n");
    pop_interp_context();
  } else if (op->opcode == ZEND_DECLARE_FUNCTION) {
    PRINT("  === declare function\n");
  } else if (op->opcode == ZEND_DECLARE_LAMBDA_FUNCTION) {
    PRINT("  === ZEND_DECLARE_LAMBDA_FUNCTION\n");
  } else if (op->opcode == ZEND_INIT_FCALL) {
    PRINT("  === ZEND_INIT_FCALL\n");
  } else if (op->opcode == ZEND_INIT_NS_FCALL_BY_NAME) {
    PRINT("  === ZEND_INIT_NS_FCALL_BY_NAME\n");
  } else if (op->opcode == ZEND_EXT_FCALL_BEGIN) {
    PRINT("  === ZEND_EXT_FCALL_BEGIN\n");
  } else if (op->opcode == ZEND_EXT_FCALL_END) {
    PRINT("  === ZEND_EXT_FCALL_END\n");
  }
}

static void opcode_compiling(const zend_op *op, uint index)
{
  PRINT("Compiling opcode %s at %d\n", zend_get_opcode_name(op->opcode), index);

  /*
  switch (op->opcode) {
    case ZEND_DECLARE_FUNCTION:
    //case ZEND_DECLARE_LAMBDA_FUNCTION:
    //case ZEND_RECV:
      PRINT("[skip compilation of %s]\n", zend_get_opcode_name(op->opcode));
      // index gets out of sync here
      break;
    default:
      add_compiled_opcode(op->opcode, index);
  }
  */
  add_compiled_opcode(op->opcode, index);
}

static void edge_compiling(uint from_index, uint to_index)
{
  add_compiled_edge(from_index, to_index);
}

static void file_compiling(const char *path)
{
  if (path == NULL)
    push_eval(eval_id);
  else
    push_compilation_unit(path);
}

static void file_compiled()
{
  control_flow_metadata_t compiled_cfm = pop_compilation_unit();
  set_interp_cfm(compiled_cfm);
}

static void function_compiling(const char *function_name)
{
  push_compilation_function(function_name);
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
