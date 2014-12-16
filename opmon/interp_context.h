#ifndef INTERP_CONTEXT_H
#define INTERP_CONTEXT_H 1

#include "cfg.h"

void initialize_interp_context();
void push_interp_context(zend_op* context, uint branch_index, cfg_t *cfg);
void set_interp_cfg(cfg_t *cfg);
void pop_interp_context();
void verify_interp_context(zend_op *context, zend_op *op, uint index);
//const char *get_current_interp_context_name();
//void set_pending_interp_context_name(const char *name);

#endif
