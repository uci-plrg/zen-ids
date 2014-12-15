#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include "php.h"
#include "zend_types.h"
#include "event_handler.h"
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

static zend_opcode_monitor_t monitor_functions;


PHP_MINIT_FUNCTION(opcode_monitor)
{
  PRINT("Initializing the opcode monitor\n");

  init_event_handler(monitor_functions);
  register_opcode_monitor(&monitor_functions);
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
