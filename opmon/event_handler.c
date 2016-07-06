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
static opmon_run_type run_type = OPMON_RUN_EXECUTION;

static void init_top_level_script(const char *script_path)
{
  starting_script(script_path);
}

static void init_worker()
{
  worker_startup();
}

static void request_boundary(bool is_request_start)
{
  uint64 request_id = interp_request_boundary(is_request_start);
  cfg_request_boundary(is_request_start, request_id);

  if (is_request_start) {
    metadata_start_request();
  } else {
    scfree_request();
    taint_clear();
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

static zend_bool zval_has_taint(const zval *value)
{
  return taint_var_get(value) != NULL;
}

zend_bool nop_notify_dataflow(const zval *src, const char *src_name,
                                     const zval *dst, const char *dst_name,
                                     zend_bool is_internal_transfer)
{
  return false;
}

zend_bool nop_has_taint(const zval *value)
{
  return false;
}

void nop_opcode_executing(const zend_op *op, uint32_t stack_motion)
{
}

void nop_notify_function_created(zend_op_array *src, zend_op_array *dst)
{
}

void nop_notify_zval_free(const zval *zv)
{
}

//static void nop_notify_http_request(zend_bool start);

monitor_query_flags_t nop_notify_database_query(const char *query)
{
  return 0;
}

void nop_notify_database_fetch(uint32_t field_count, const char **table_names,
                                      const char **column_names, const zval **value)
{
}

void init_event_handler(zend_opcode_monitor_t *monitor)
{
  // scarray_unit_test();

  if (IS_CFI_DB() && !IS_REQUEST_ID_SYNCH_DB()) {
    ERROR("The cfi mode is incompatible with the request id synch!\n");
    return;
  }
  if (IS_CFI_FILE() && !IS_REQUEST_ID_SYNCH_FILE()) {
    ERROR("The cfi mode is incompatible with the request id synch!\n");
    return;
  }

  static_analysis = getenv(ENV_STATIC_ANALYSIS);
  SPOT("static_analysis: %s\n", static_analysis);
  if (static_analysis != NULL)
    run_type = OPMON_RUN_STATIC_ANALYSIS;

  init_utils();
  init_compile_context();
  init_cfg_handler();
  init_metadata_handler();
  init_taint_tracker();

  if (IS_OPCODE_DUMP_ENABLED())
    init_dataflow_analysis();

  monitor->set_top_level_script = init_top_level_script;
  monitor->notify_worker_startup = init_worker;
  monitor->opmon_tokenize = NULL; //tokenize_file;
  monitor->opmon_dataflow = start_dataflow_analysis;
  monitor->notify_http_request = request_boundary;
  if (false) {
    monitor->has_taint = nop_has_taint;
    monitor->notify_opcode_interp = nop_opcode_executing;
    monitor->notify_function_created = nop_notify_function_created;
    monitor->dataflow.notify_dataflow = nop_notify_dataflow;
    monitor->notify_zval_free = nop_notify_zval_free;
    monitor->notify_database_fetch = nop_notify_database_fetch;
    monitor->notify_database_query = nop_notify_database_query;
  } else if (false) {
    monitor->has_taint = zval_has_taint;
    monitor->notify_opcode_interp = nop_opcode_executing;
    monitor->notify_function_created = function_created;
    monitor->dataflow.notify_dataflow = internal_dataflow;
    monitor->notify_zval_free = taint_var_free;
    monitor->notify_database_fetch = db_fetch_trigger;
    monitor->notify_database_query = db_query;
  } else {
    monitor->has_taint = zval_has_taint;
    monitor->notify_opcode_interp = opcode_executing;
    monitor->notify_function_created = function_created;
    monitor->dataflow.notify_dataflow = internal_dataflow;
    monitor->notify_zval_free = taint_var_free;
    monitor->notify_database_fetch = db_fetch_trigger;
    monitor->notify_database_query = db_query;
  }

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
