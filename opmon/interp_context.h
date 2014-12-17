#ifndef INTERP_CONTEXT_H
#define INTERP_CONTEXT_H 1

#include "cfg.h"

void initialize_interp_context();
void push_interp_context(zend_op* context, uint branch_index, routine_cfg_t *cfg);
void set_interp_cfg(routine_cfg_t *cfg);
void pop_interp_context();
void verify_interp_context(zend_op *context, cfg_node_t node);

#endif
