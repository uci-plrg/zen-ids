#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_ 1

#include "interp_context.h"
#include "lib/script_cfi_utils.h"

void init_event_handler();
void destroy_event_handler();
const char *get_static_analysis();
bool is_static_analysis();
bool is_dataflow_analysis();
void enable_request_taint_tracking(bool enabled);
void enable_monitor(bool enabled);

#endif
