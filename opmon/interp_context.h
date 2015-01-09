#ifndef INTERP_CONTEXT_H
#define INTERP_CONTEXT_H 1

#include "metadata_handler.h"
#include "cfg.h"

void initialize_interp_context();
const char *get_current_interp_context_name();
routine_cfg_t *get_current_interp_routine_cfg();
void push_interp_context(zend_op* op_array, uint branch_index, control_flow_metadata_t cfm);
void set_interp_cfm(control_flow_metadata_t cfm);
void pop_interp_context();
void verify_interp_context(zend_op *context, cfg_node_t node);

#endif
