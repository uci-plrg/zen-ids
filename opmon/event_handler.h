#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_ 1

#include "interp_context.h"
#include "lib/script_cfi_utils.h"

void init_event_handler(zend_opcode_monitor_t *monitor);
void destroy_event_handler();
const char *get_static_analysis();
bool is_static_analysis();
bool is_dataflow_analysis();
void set_interp_routine(execute_opcode_t loop);

#endif
