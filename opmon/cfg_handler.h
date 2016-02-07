#ifndef _CFG_HANDLER_H_
#define _CFG_HANDLER_H_ 1

#include "httpd.h"

#include "cfg.h"
#include "taint.h"

void init_cfg_handler();
void destroy_cfg_handler();

void starting_script(const char *script_path);
void worker_startup();
void server_startup();

void cfg_initialize_application(application_t *app);
void cfg_destroy_application(application_t *app);
void cfg_request(bool start);
void cfg_form(const char *form_data, int length);

void write_node(application_t *app, uint routine_hash, cfg_opcode_t *opcode, uint index);
void write_op_edge(application_t *app, uint routine_hash, uint from_index, uint to_index,
                   user_level_t user_level);
void write_routine_edge(bool is_new_in_process, application_t *app, uint from_routine_hash,
                        uint from_index, uint to_routine_hash, uint to_index,
                        user_level_t user_level);
void write_routine_catalog_entry(application_t *app, uint routine_hash, const char *unit_path,
                                 const char *routine_name);

void print_var_value(FILE *out, const zval *var);
void print_operand(FILE *out, const char *tag, zend_op_array *ops,
                   const znode_op *operand, const zend_uchar type);
void plog_builtin(application_t *app, zend_op_array *op_array, const zend_op *call_op);
void plog_taint(application_t *app, taint_variable_t *taint);

void flush_all_outputs(application_t *app);

int get_current_request_id();

#endif
