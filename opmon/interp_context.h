#ifndef INTERP_CONTEXT_H
#define INTERP_CONTEXT_H 1

void initialize_interp_context();
void push_interp_context(zend_op* context, uint branch_index);
void pop_interp_context();
void verify_interp_context(zend_op* context, uint index);
const char *get_current_interp_context_name();
void set_pending_interp_context_name(const char *name);

#endif
