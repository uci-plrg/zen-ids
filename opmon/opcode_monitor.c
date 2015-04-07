#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include "php.h"
#include "zend_types.h"
#include "event_handler.h"
#include "interp_context.h"
#include "php_opcode_monitor.h"
#include "lib/script_cfi_hashtable.h"
#include "lib/script_cfi_utils.h"

#define EC(f) opcode_monitor_globals.execution_context.f

PHP_INI_BEGIN()
  STD_PHP_INI_ENTRY("opmon_dataset_dir", ".", PHP_INI_PERDIR, OnUpdateString,
                    dataset_dir, zend_opcode_monitor_globals, opcode_monitor_globals)
  STD_PHP_INI_ENTRY("opmon_verbose", ".", PHP_INI_PERDIR, OnUpdateLong,
                    verbose, zend_opcode_monitor_globals, opcode_monitor_globals)
PHP_INI_END()

static PHP_GINIT_FUNCTION(opcode_monitor);

PHP_FUNCTION(set_user_level)
{
  int argc = ZEND_NUM_ARGS();
  long user_level;
  long blog_id;
  if (zend_parse_parameters(argc TSRMLS_CC, "ll", &user_level, &blog_id) == FAILURE) {
    ERROR("Failed to parse parameters in a call to set_user_level()\n");
    return;
  }

  if (is_php_session_active()) {
    zend_string *key = zend_string_init(USER_SESSION_KEY, sizeof(USER_SESSION_KEY) - 1, 0);
    zval *session_zval = php_get_session_var(key);
    if (session_zval == NULL || Z_TYPE_INFO_P(session_zval) != IS_LONG) {
      session_zval = malloc(sizeof(zval));
      ZVAL_LONG(session_zval, user_level);
      session_zval = php_session_set_var(key, session_zval);
      PRINT("<session> No user session during set_user_level--created new session user with level %ld\n", user_level);
    } else {
      PRINT("<session> Found session user level %ld during set_user_level\n", Z_LVAL_P(session_zval));
      Z_LVAL_P(session_zval) = user_level;
      zend_string_release(key);
    }
    PRINT("<session> Set session user with level %ld on pid 0x%x\n", Z_LVAL_P(session_zval), getpid());
  } else {
    ERROR("<session> User level assigned with no active PHP session!\n");
  }

  PRINT("<session> ScriptCFI receives a call to set_user_level to %ld on blog %ld on pid 0x%x\n",
       user_level, blog_id, getpid());
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_set_user_level, 0, 0, 2)
  ZEND_ARG_INFO(0, level)
  ZEND_ARG_INFO(0, blog_id)
ZEND_END_ARG_INFO()

static zend_function_entry php_opcode_monitor_functions[] = {
  PHP_FE(set_user_level, arginfo_set_user_level)
  {NULL, NULL, NULL}
};

zend_module_entry opcode_monitor_module_entry = {
  STANDARD_MODULE_HEADER,        /* 6 members */
  PHP_OPCODE_MONITOR_EXTNAME,
  php_opcode_monitor_functions,
  PHP_MINIT(opcode_monitor),     /* module startup */
  PHP_MSHUTDOWN(opcode_monitor), /* module shutdown */
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
  REGISTER_INI_ENTRIES();

  //PRINT("INI example: dataset dir is %s\n", INI_STR("opmon_dataset_dir"));
  //PRINT("INI example: dataset dir is %s\n", OPMON_G(dataset_dir));

  if (strlen(OPMON_G(dataset_dir)) > 200)
    ERROR("dataset dirname is too long. Please rebuild with a larger buffer.\n");

  init_event_handler(&monitor_functions);
  register_opcode_monitor(&monitor_functions);
  initialize_interp_context();

  return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(opcode_monitor)
{
  UNREGISTER_INI_ENTRIES();

  destroy_event_handler();

  return SUCCESS;
}

PHP_MINFO_FUNCTION(opcode_monitor)
{
  DISPLAY_INI_ENTRIES();
}

static PHP_GINIT_FUNCTION(opcode_monitor)
{
  opcode_monitor_globals->execution_context.foo = 3;
}
