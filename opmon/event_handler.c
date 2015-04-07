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

typedef struct _pending_load_t {
  bool pending_execution;
  char function_name[MAX_FUNCTION_NAME];
} pending_load_t;

/*
static cfg_node_t last_executed_node = { ZEND_NOP, 0 };
static zend_op *current_op_array = NULL;
static zend_op *last_op_array = NULL;
*/

static uint pending_cfm_frame;
//static control_flow_metadata_t *call_to_eval = (control_flow_metadata_t *)int2p(1);
static control_flow_metadata_t *initial_context = (control_flow_metadata_t *)int2p(2);

static control_flow_metadata_t *pending_cfm_stack[MAX_STACK_FRAME];

//static char last_unknown_function_name[MAX_FUNCTION_NAME];
static pending_load_t pending_load;

static const char *static_analysis;

cfg_t *app_cfg;

static inline void pend_cfm(control_flow_metadata_t *cfm)
{
  pending_cfm_stack[pending_cfm_frame++] = cfm;
  if (pending_cfm_frame >= MAX_STACK_FRAME)
    ERROR("pending_cfm_frame exceeds max stack frame!\n");
}

static inline control_flow_metadata_t *pop_cfm()
{
  pending_cfm_frame--;
  if (pending_cfm_frame == 0)
    ERROR("pending_cfm_frame hit stack bottom!\n");
  return pending_cfm_stack[pending_cfm_frame];
}

static inline control_flow_metadata_t *peek_cfm()
{
  return pending_cfm_stack[pending_cfm_frame-1];
}

static void init_top_level_script(const char *script_path)
{
  starting_script(script_path);
  load_entry_point_dataset();
}

static void init_server()
{
  server_startup();
  load_entry_point_dataset();
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
  //extern foo *foobar;
  //scarray_unit_test();

  static_analysis = getenv(ENV_STATIC_ANALYSIS);

  app_cfg = cfg_new();

  pending_cfm_stack[0] = NULL;
  pending_cfm_stack[1] = NULL;
  pending_cfm_frame = 1;
  pend_cfm(initial_context);

  pending_load.pending_execution = false;

  init_compile_context();
  init_cfg_handler();
  init_metadata_handler();

  monitor->set_top_level_script = init_top_level_script;
  monitor->notify_opcode_interp = opcode_executing;
  monitor->notify_function_compile_complete = function_compiled;
  monitor->notify_worker_startup = worker_startup;

  SPOT("SAPI type: %s\n", EG(sapi_type));

  if (strcmp(EG(sapi_type), "apache2handler") == 0)
    init_server();
}

void destroy_event_handler()
{
  destroy_cfg_handler();
  destroy_metadata_handler();
  destroy_operand_resolver();
}
