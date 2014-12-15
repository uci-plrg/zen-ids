#include "php_opcode_monitor.h"
#include "interp_context.h"
#include "compile_context.h"
#include "lib/script_cfi_utils.h"
#include "event_handler.h"

// todo: thread safety?
static uint eval_id = EVAL_FLAG + 1;

static void opcode_executing(const zend_op *op)
{
  uint op_index;
  zend_op *current_opcodes;
  uint hash;

  if (EG(current_execute_data) != NULL && EG(current_execute_data)->func != NULL)
    current_opcodes = EG(current_execute_data)->func->op_array.opcodes;

  if (current_opcodes == NULL)
    op_index = 0xffffffff;
  else
    op_index = (uint)(op - current_opcodes);

  PRINT("[%s(0x%x):%d, line %d]: 0x%x:%s\n", get_current_interp_context_name(), get_current_interp_context_id(), 
    op_index, op->lineno, op->opcode, zend_get_opcode_name(op->opcode));
  
  verify_interp_context(current_opcodes, op_index);

  if (op->opcode == ZEND_INCLUDE_OR_EVAL) {
    switch (op->extended_value) {
      case ZEND_EVAL: {
        PRINT("  === entering `eval` context #%u\n", eval_id & ~EVAL_FLAG); 
        set_staged_interp_context(eval_id++);
      } break;
      /*
      case ZEND_INCLUDE:
      case ZEND_INCLUDE_ONCE: {
        zval temp_filename, *inc_filename = op->op1.zv;
        ZVAL_UNDEF(&temp_filename);
        if (Z_TYPE_P(inc_filename) != IS_STRING) {
          ZVAL_STR(&temp_filename, zval_get_string(inc_filename));
          inc_filename = &temp_filename;
        }
        hash = HASH_STRING(Z_STRVAL_P(inc_filename));
        PRINT("  === entering `include` context for %s(0x%x)\n", Z_STRVAL_P(inc_filename), hash); 
        set_staged_interp_context(hash);
      } break;
      case ZEND_REQUIRE:
      case ZEND_REQUIRE_ONCE: {
        zval temp_filename, *inc_filename = op->op1.zv;
        ZVAL_UNDEF(&temp_filename);
        if (Z_TYPE_P(inc_filename) != IS_STRING) {
          ZVAL_STR(&temp_filename, zval_get_string(inc_filename));
          inc_filename = &temp_filename;
        }
        hash = HASH_STRING(Z_STRVAL_P(inc_filename));
        PRINT("  === entering `require` context for %s(0x%x)\n", Z_STRVAL_P(inc_filename), hash); 
        set_staged_interp_context(hash);
      } break;
      default: {
        PRINT("  === entering unknown context\n");
        set_staged_interp_context(UNKNOWN_CONTEXT_ID);
      }
      */
    }
    push_interp_context(current_opcodes, op_index);
  } else if (op->opcode == ZEND_INIT_FCALL_BY_NAME) {
    PRINT("  === init call to function %s\n", op->op2.zv->value.str->val);
    // lookup FQN (file_path|function_name) by function name
    set_staged_interp_context(HASH_STRING(op->op2.zv->value.str->val)); 
    // can't see this in ZEND_DO_FCALL... need to pend the context
  } else if (op->opcode == ZEND_DO_FCALL) {
    push_interp_context(current_opcodes, op_index);
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

static void opcode_compiling(const zend_op *op)
{
  switch (op->opcode) {
    case ZEND_DECLARE_FUNCTION:
    case ZEND_DECLARE_LAMBDA_FUNCTION:
    case ZEND_RECV:
      break;
    default:
      PRINT("[emit %s for {%s|%s, 0x%x|0x%x}]\n", zend_get_opcode_name(op->opcode),
            get_compilation_unit_path(), get_compilation_function_name(),
            get_compilation_unit_hash(), get_compilation_function_hash());
  }
}

static void file_compiling(const char *path)
{
  push_compilation_unit(path);
}

static void file_compiled()
{
  pop_compilation_unit();
}

static void function_compiling(const char *path)
{
  push_compilation_function(path);
}

static void function_compiled()
{
  pop_compilation_function();
}

void init_event_handler(zend_opcode_monitor_t *monitor)
{
  init_compile_context();
  
  monitor->notify_opcode_interp = opcode_executing;
  monitor->notify_opcode_compile = opcode_compiling;
  monitor->notify_file_compile_start = file_compiling;
  monitor->notify_file_compile_complete = file_compiled;
  monitor->notify_function_compile_start = function_compiling;
  monitor->notify_function_compile_complete = function_compiled;
}

