#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_ 1

#include "interp_context.h"
#include "lib/script_cfi_utils.h"

typedef enum _monitor_mode_t {
  MONITOR_MODE_NONE,
  MONITOR_MODE_CALLS,
  MONITOR_MODE_ALL,
} monitor_mode_t;

void init_event_handler();
void destroy_event_handler();
const char *get_static_analysis();
bool is_static_analysis();
bool is_dataflow_analysis();
void set_monitor_mode(monitor_mode_t mode);

#endif
