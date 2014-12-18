#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include "php.h"
#include "zend_types.h"
#include "event_handler.h"
#include "php_opcode_monitor.h"
#include "lib/script_cfi_hashtable.h"
#include "lib/script_cfi_utils.h"

#define EC(f) opcode_monitor_globals.execution_context.f

typedef struct _execution_context_t {
  //const zend_op *base_op;
  uint foo;
} execution_context_t;

ZEND_BEGIN_MODULE_GLOBALS(opcode_monitor)
  execution_context_t execution_context;
  const char *dataset_dir;
ZEND_END_MODULE_GLOBALS(opcode_monitor)

#ifdef ZTS
# define OPMON_G(v) TSRMG(opcode_monitor_globals, zend_opcode_monitor_globals *, v)
#else
# define OPMON_G(v) (opcode_monitor_globals.v)
#endif

ZEND_DECLARE_MODULE_GLOBALS(opcode_monitor)

PHP_INI_BEGIN()
  STD_PHP_INI_ENTRY("opmon_dataset_dir", ".", PHP_INI_PERDIR, OnUpdateString, 
                    dataset_dir, zend_opcode_monitor_globals, opcode_monitor_globals)
PHP_INI_END()

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
  
  REGISTER_INI_ENTRIES();
  
  PRINT("INI example: dataset dir is %s\n", INI_STR("opmon_dataset_dir"));
  PRINT("INI example: dataset dir is %s\n", OPMON_G(dataset_dir));

  init_event_handler(&monitor_functions);
  register_opcode_monitor(&monitor_functions);
  initialize_interp_context();
}

PHP_MSHUTDOWN_FUNCTION(opcode_monitor)
{
  UNREGISTER_INI_ENTRIES();
}

PHP_MINFO_FUNCTION(opcode_monitor)
{
  DISPLAY_INI_ENTRIES();
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
