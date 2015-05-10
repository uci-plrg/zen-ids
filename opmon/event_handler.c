#include "php_opcode_monitor.h"
#include "interp_context.h"
#include "compile_context.h"
#include "cfg_handler.h"
#include "cfg.h"
#include "operand_resolver.h"
#include "lib/script_cfi_utils.h"
#include "lib/script_cfi_array.h"
#include "event_handler.h"

#define MAX_FUNCTION_NAME 256
#define MAX_STACK_FRAME 256
#define ENV_STATIC_ANALYSIS "OPMON_STATIC_ANALYSIS"

#define CHECK_FUNCTION_NAME_LENGTH(len) \
do { \
  if ((len) >= MAX_FUNCTION_NAME) \
    ERROR("function_name exceeds max length 256!\n"); \
} while (0)

static const char *static_analysis;

static void init_top_level_script(const char *script_path)
{
  starting_script(script_path);
}

static void init_worker()
{
  worker_startup();
}

const char *get_static_analysis()
{
  return static_analysis;
}

bool is_static_analysis()
{
  return static_analysis != NULL;
}

void init_event_handler(zend_opcode_monitor_t *monitor)
{
  // scarray_unit_test();

  static_analysis = getenv(ENV_STATIC_ANALYSIS);

  init_compile_context();
  init_cfg_handler();
  init_metadata_handler();

  monitor->set_top_level_script = init_top_level_script;
  monitor->notify_opcode_interp = opcode_executing;
  monitor->notify_function_compile_complete = function_compiled;
  monitor->notify_request = cfg_request;
  monitor->notify_worker_startup = init_worker;
  monitor->opmon_tokenize = tokenize_file;

  SPOT("SAPI type: %s\n", EG(sapi_type));

  if (strcmp(EG(sapi_type), "apache2handler") == 0)
    server_startup();
}

void destroy_event_handler()
{
  destroy_metadata_handler();
  destroy_operand_resolver();
  destroy_cfg_handler();
}
