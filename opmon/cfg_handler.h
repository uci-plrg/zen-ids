#ifndef _CFG_HANDLER_H_
#define _CFG_HANDLER_H_ 1

#include "httpd.h"

#include "cfg.h"
#include "taint.h"

#define PLOG_TAINT 1
#define PLOG_CFG 1
#define PLOG_CFG_BLOCK 1
//#define PLOG_CFG_DETAIL 1
#define PLOG_DB 1
#define PLOG_DB_MOD 1
//#define PLOG_FILE_MOD 1
//#define PLOG_FILE_OUTPUT 1
//#define PLOG_SYS_WRITE 1
//#define PLOG_SYS_READ 1
//#define PLOG_WARN 1
//#define PLOG_AD_HOC 1

typedef struct _php_server_context_t { // type window
  int opaque;
  request_rec *r;
  apr_bucket_brigade *bb;
} php_server_context_t;

typedef struct _cfg_files_t {
  FILE *node;
  FILE *op_edge;
  FILE *routine_edge;
  FILE *request;
  FILE *request_edge;
  FILE *routine_catalog;
  FILE *persistence;
  FILE *opcode_log;
} cfg_files_t;

typedef enum _plog_type_t {
  PLOG_TYPE_TAINT,
  PLOG_TYPE_CFG,
  PLOG_TYPE_CFG_BLOCK,
  PLOG_TYPE_CFG_DETAIL,
  PLOG_TYPE_DB,
  PLOG_TYPE_DB_MOD,
  PLOG_TYPE_FILE_MOD,
  PLOG_TYPE_FILE_OUTPUT,
  PLOG_TYPE_SYS_WRITE,
  PLOG_TYPE_WARN,
  PLOG_TYPE_AD_HOC,
} plog_type_t;

void init_cfg_handler();
void destroy_cfg_handler();

void starting_script(const char *script_path);
void worker_startup();
void server_startup();

void cfg_initialize_application(application_t *app);
void cfg_destroy_application(application_t *app);
bool is_standalone_mode();
void cfg_request_boundary(bool is_first, uint64 request_id);
void cfg_form(const char *form_data, int length);

void write_node(application_t *app, uint routine_hash, cfg_opcode_t *opcode, uint index);
void write_op_edge(application_t *app, uint routine_hash, uint from_index, uint to_index,
                   user_level_t user_level);
bool write_request_edge(bool is_new_in_process, application_t *app, uint from_routine_hash,
                        uint from_index, uint to_routine_hash, uint to_index,
                        user_level_t user_level);
void write_routine_edge(application_t *app, uint from_routine_hash, uint from_index,
                        uint to_routine_hash, uint to_index, user_level_t user_level);
void write_routine_catalog_entry(application_t *app, uint routine_hash, const char *unit_path,
                                 const char *routine_name);

void print_var_value(FILE *out, const zval *var);
void print_operand_value(FILE *out, const znode_op *operand);
void print_operand(FILE *out, const char *tag, zend_op_array *ops,
                   const znode_op *operand, const zend_uchar type);

#ifdef TAINT_IO
void plog_call(application_t *app, const char *tag, const char *callee_name,
               zend_op_array *op_array, const zend_op *call_op,
               uint arg_count, const zend_op **args);
#endif

#ifdef PLOG_TAINT
void plog_taint(application_t *app, taint_variable_t *taint_var);
void plog_taint_var(application_t *app, taint_variable_t *taint_var, uint64 hash);
#endif

/* print the tag */
void plog(application_t *app, plog_type_t type, const char *message, ...);
/* without printing the tag */
void plog_append(application_t *app, plog_type_t type, const char *message, ...);
void plog_call(zend_execute_data *execute_data, application_t *app, plog_type_t type,
               const char *callee_name, const zend_op **args, uint arg_count);
void plog_stacktrace(application_t *app, plog_type_t type, zend_execute_data *start_frame);

void flush_all_outputs(application_t *app);

bool is_stateful_syscall(const char *callee_name);

int get_current_request_id();
uint64 get_current_request_start_time();
const char *get_current_request_address();

#endif
