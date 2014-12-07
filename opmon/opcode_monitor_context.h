#ifndef PHP_OPCODE_MONITOR_CONTEXT_H
#define PHP_OPCODE_MONITOR_CONTEXT_H 1

void initialize_opcode_monitor_context();
void push_context(zend_op* context, uint branch_index);
void pop_context();
void verify_context(zend_op* context, uint index);
const char *get_current_context_name();
void set_pending_context_name(const char *name);

#endif
