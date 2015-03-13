#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_ 1

void init_event_handler(zend_opcode_monitor_t *monitor);
const char *get_static_analysis();
bool is_static_analysis();

#endif
