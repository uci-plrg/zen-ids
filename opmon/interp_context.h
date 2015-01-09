#ifndef INTERP_CONTEXT_H
#define INTERP_CONTEXT_H 1

#include "metadata_handler.h"
#include "cfg.h"

void initialize_interp_context();
void routine_call(zend_execute_data *call);
void push_interp_context(zend_op* op_array, uint branch_index, control_flow_metadata_t cfm);
void pop_interp_context();
void opcode_executing(const zend_op *op);

#endif
