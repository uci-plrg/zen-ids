#ifndef INTERP_CONTEXT_H
#define INTERP_CONTEXT_H 1

#include "metadata_handler.h"
#include "cfg.h"

void initialize_interp_context();
void initialize_interp_app_context(application_t *app);
void destroy_interp_app_context(application_t *app);

void push_interp_context(zend_op* op_array, uint branch_index, control_flow_metadata_t cfm);
void pop_interp_context();
void opcode_executing(const zend_op *op);

void db_site_modification(const zval *value, const char *table_name, const char *column_name);
void internal_dataflow(const zval *src, const char *src_name, const zval *dst, const char *dst_name);
user_level_t get_current_user_level();

#endif
