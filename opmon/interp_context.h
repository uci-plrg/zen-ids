#ifndef INTERP_CONTEXT_H
#define INTERP_CONTEXT_H 1

#include <php.h>
#include "metadata_handler.h"
#include "cfg.h"
// #include "interp_handler.h"

typedef enum _stack_motion_t {
  STACK_MOTION_RETURN = -1,
  STACK_MOTION_NONE = 0,
  STACK_MOTION_CALL = 1,
  STACK_MOTION_LEAVE = 2,
} stack_motion_t;

typedef void (*execute_opcode_t)(zend_execute_data *execute_data TSRMLS_DC);

void initialize_interp_context();
void initialize_interp_app_context(application_t *app);
void destroy_interp_app_context(application_t *app);
uint64 interp_request_boundary(bool is_first);
void set_opmon_user_level(long user_level);

void push_interp_context(zend_op* op_array, uint branch_index, control_flow_metadata_t cfm);
void pop_interp_context();
void monitor_call();
void monitor_call_quick();
void execute_opcode_monitor_all(zend_execute_data *execute_data TSRMLS_DC);
void execute_opcode_monitor_calls(zend_execute_data *execute_data TSRMLS_DC);
void execute_opcode_direct(zend_execute_data *execute_data TSRMLS_DC);

void db_fetch_trigger(uint32_t field_count, const char **table_names, const char **column_names,
                      const zval **values);
monitor_query_flags_t db_query(const char *query);
zend_bool internal_dataflow(const zval *src, const char *src_name,
                            const zval *dst, const char *dst_name,
                            zend_bool is_internal_transfer);
user_level_t get_current_user_level();

#endif
