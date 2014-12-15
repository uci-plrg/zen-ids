#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include "php.h"
#include "zend_types.h"
#include "interp_context.h"
#include "compile_context.h"
#include "php_opcode_monitor.h"
#include "../lib/script_cfi_hashtable.h"

#define EVAL_FLAG 0x80000000U
#define UNKNOWN_CONTEXT_ID 0xffffffffU

// todo: thread safety?
static uint eval_id = EVAL_FLAG + 1;

#define EC(f) opcode_monitor_globals.execution_context.f

typedef struct _execution_context_t {
  //const zend_op *base_op;
  uint foo;
} execution_context_t;

ZEND_BEGIN_MODULE_GLOBALS(opcode_monitor)
  execution_context_t execution_context;
ZEND_END_MODULE_GLOBALS(opcode_monitor)

#ifdef ZTS
# define ReSG(v) TSRMG(opcode_monitor_globals, zend_opcode_monitor_globals *, v)
#else
# define ReSG(v) (opcode_monitor_globals.v)
#endif

ZEND_DECLARE_MODULE_GLOBALS(opcode_monitor)
static PHP_GINIT_FUNCTION(opcode_monitor);

static zend_function_entry php_opcode_monitor_functions[] = {
  PHP_FE(opcode_monitor_string, NULL)
  {NULL, NULL, NULL}
};

zend_module_entry opcode_monitor_module_entry = {
  STANDARD_MODULE_HEADER,        /* 6 members */
  PHP_OPCODE_MONITOR_EXTNAME,    
  php_opcode_monitor_functions,
  PHP_MINIT(opcode_monitor),     /* module startup */
  NULL,                          /* module shutdown */
  NULL,                          /* request startup */
  NULL,                          /* request shutdown */
  NULL,                          /* info */
  PHP_OPCODE_MONITOR_VERSION,
  PHP_MODULE_GLOBALS(opcode_monitor),
  PHP_GINIT(opcode_monitor),     /* globals init */
  NULL,                          /* globals ctor */
  NULL,                          /* globals dtor */
  STANDARD_MODULE_PROPERTIES_EX  /* 6 members */
};

#ifdef COMPILE_DL_OPMON
ZEND_GET_MODULE(opcode_monitor)
#endif

static uint hash_string(const char *string)
{
  uint four;
  uint hash = 0;
  uint i = strlen(string);

  while (i > 3) {
    four = *(uint *)string;
    i -= 4;
    string += 4;
    hash = hash ^ (hash << 5) ^ four;
  }
  
  if (i > 0) {
    four = 0;
    while (i-- > 0)
      four = (four << 8) & *string++;
    hash = hash ^ (hash << 5) ^ four;
  }
  
  return hash & ~EVAL_FLAG;
}

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
        hash = hash_string(Z_STRVAL_P(inc_filename));
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
        hash = hash_string(Z_STRVAL_P(inc_filename));
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
    set_staged_interp_context(hash_string(op->op2.zv->value.str->val));
  } else if (op->opcode == ZEND_DO_FCALL) {
    push_interp_context(current_opcodes, op_index);
  } else if (op->opcode == ZEND_RETURN) {
    PRINT("  === return\n");
    pop_interp_context();
  } else if (op->opcode == ZEND_DECLARE_FUNCTION) {
    PRINT("  === declare function\n");
  } else if (op->opcode == ZEND_DECLARE_LAMBDA_FUNCTION) {
    PRINT("  === ZEND_DECLARE_LAMBDA_FUNCTION\n");
  } else if (op->opcode == ZEND_INIT_FCALL_BY_NAME) {
    PRINT("  === ZEND_INIT_FCALL_BY_NAME\n");
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

static void opcode_processing(const zend_op *op, zend_bool compiling)
{
  if (compiling) {
    switch (op->opcode) {
      case ZEND_DECLARE_FUNCTION:
      case ZEND_DECLARE_LAMBDA_FUNCTION:
      case ZEND_RECV:
        break;
      default:
        PRINT("[emit %s]\n", zend_get_opcode_name(op->opcode));
    }
  } else {
    opcode_executing(op);
  }
}

PHP_MINIT_FUNCTION(opcode_monitor)
{
  PRINT("Initializing the opcode monitor\n");

  register_opcode_monitor(opcode_processing);
  initialize_interp_context();
}

static PHP_GINIT_FUNCTION(opcode_monitor)
{
  opcode_monitor_globals->execution_context.foo = 3;
}

PHP_FUNCTION(opcode_monitor_string)
{
  PRINT("Executing the opcode monitor function\n");
  RETURN_STRING("Opcode monitor says: 'Hello World'");
}
