#include "php_opcode_monitor.h"
#include "interp_context.h"
#include "compile_context.h"
#include "cfg_handler.h"
#include "cfg.h"
#include "dataflow.h"
#include "taint.h"
#include "operand_resolver.h"
#include "lib/script_cfi_utils.h"
#include "lib/script_cfi_array.h"
#include "event_handler.h"

#define MAX_FUNCTION_NAME 256
#define MAX_STACK_FRAME 256
#define ENV_STATIC_ANALYSIS "OPMON_STATIC_ANALYSIS"
#define ENV_OPCODE_DUMP "OPMON_OPCODE_DUMP"
#define ENV_TAINT_ANALYSIS "OPMON_TAINT_ANALYSIS"

#define CHECK_FUNCTION_NAME_LENGTH(len) \
do { \
  if ((len) >= MAX_FUNCTION_NAME) \
    ERROR("function_name exceeds max length 256!\n"); \
} while (0)

typedef enum _opmon_run_type {
  OPMON_RUN_EXECUTION,
  OPMON_RUN_STATIC_ANALYSIS,
  OPMON_RUN_DATAFLOW_ANALYSIS
} opmon_run_type;

static const char *static_analysis = NULL;
static bool opcode_dump_enabled = false;
static bool taint_analysis_enabled = false;
static opmon_run_type run_type = OPMON_RUN_EXECUTION;
static void init_top_level_script(const char *script_path)
{
  starting_script(script_path);
}

static void init_worker()
{
  worker_startup();
}

static void request_boundary(bool is_first)
{
  cfg_request(is_first);

  if (!is_first) {
    scfree_request();
    taint_clear();
    implicit_taint_clear();
  }
}

const char *get_static_analysis()
{
  return static_analysis;
}

bool is_static_analysis()
{
  return run_type == OPMON_RUN_STATIC_ANALYSIS;
}

static int start_dataflow_analysis(zend_file_handle *file)
{
  if (run_type != OPMON_RUN_EXECUTION) {
    ERROR("Cannot perform static analysis and dataflow analysis at the same time!\n");
    return FAILURE;
  }

  run_type = OPMON_RUN_DATAFLOW_ANALYSIS;
  return analyze_dataflow(NULL, file);
}

bool is_dataflow_analysis()
{
  return run_type == OPMON_RUN_DATAFLOW_ANALYSIS;
}

bool is_opcode_dump_enabled()
{
  return opcode_dump_enabled;
}

bool is_taint_analysis_enabled()
{
  return taint_analysis_enabled;
}

static zend_bool zval_has_taint(const zval *value)
{
  return taint_var_get(value) != NULL;
}

void init_event_handler(zend_opcode_monitor_t *monitor)
{
  // scarray_unit_test();

  static_analysis = getenv(ENV_STATIC_ANALYSIS);
  SPOT("static_analysis: %s\n", static_analysis);
  if (static_analysis != NULL)
    run_type = OPMON_RUN_STATIC_ANALYSIS;

  opcode_dump_enabled = (getenv(ENV_OPCODE_DUMP) != NULL);
  taint_analysis_enabled = (getenv(ENV_TAINT_ANALYSIS) != NULL);

  init_utils();
  init_compile_context();
  init_cfg_handler();
  init_metadata_handler();
  init_taint_tracker();

  if (opcode_dump_enabled)
    init_dataflow_analysis();

  monitor->set_top_level_script = init_top_level_script;
  monitor->has_taint = zval_has_taint;
  monitor->notify_opcode_interp = opcode_executing;
  monitor->notify_function_compile_complete = function_compiled;
  monitor->dataflow.notify_dataflow = internal_dataflow;
  monitor->notify_zval_free = taint_var_free;
  monitor->notify_http_request = request_boundary;
  monitor->notify_database_site_modification = db_site_modification;
  monitor->notify_database_fetch = db_fetch;
  monitor->notify_database_query = db_query;
  monitor->current_user_is_admin = is_admin;
  monitor->notify_worker_startup = init_worker;
  monitor->opmon_tokenize = NULL; //tokenize_file;
  monitor->opmon_dataflow = start_dataflow_analysis;

  SPOT("SAPI type: %s\n", EG(sapi_type));

  if (strcmp(EG(sapi_type), "apache2handler") == 0)
    server_startup();
}

void destroy_event_handler()
{
  destroy_utils();
  destroy_metadata_handler();
  destroy_operand_resolver();
  destroy_cfg_handler();
  destroy_taint_tracker();

  if (run_type == OPMON_RUN_DATAFLOW_ANALYSIS)
    destroy_dataflow_analysis();
}
