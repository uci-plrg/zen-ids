#ifndef _TAINT_H_
#define _TAINT_H_ 1

#include "php.h"
#include "cfg.h"

// #define TAINT_IO 1

typedef enum _site_modification_type_t {
  SITE_MOD_NONE,
  SITE_MOD_DB,
  SITE_MOD_FILE
} site_modification_type_t;

typedef struct _site_modification_t {
  site_modification_type_t type;
  union {
    const char *file_path;
    const char *db_table;
  };
  const char *db_column;
  const char *db_value;
  void *source;
} site_modification_t;

typedef enum _request_input_type_t {
  REQUEST_INPUT_TYPE_NONE,
  REQUEST_INPUT_TYPE_REQUEST,
  REQUEST_INPUT_TYPE_GET,
  REQUEST_INPUT_TYPE_POST,
  REQUEST_INPUT_TYPE_COOKIE,
  REQUEST_INPUT_TYPE_SERVER,
  REQUEST_INPUT_TYPE_FILES,
} request_input_type_t;

typedef struct _request_input_t {
  request_input_type_t type;
  zval *value; /* NULL indicates fetch of the superglobal itself */
} request_input_t;

typedef enum _taint_type_t {
  TAINT_TYPE_SITE_MOD,
#ifdef TAINT_IO
  TAINT_TYPE_REQUEST_INPUT
#endif
} taint_type_t;

typedef enum _taint_variable_type_t {
  TAINT_VAR_NONE,
  TAINT_VAR_TEMP,
  TAINT_VAR_LOCAL,
  TAINT_VAR_GLOBAL
} taint_variable_type_t;

typedef union _taint_variable_id_t {
  uint temp_id;
  const char *var_name;
} taint_variable_id_t;

typedef struct _taint_variable_t {
  //taint_variable_type_t var_type;
  //taint_variable_id_t var_id;
  // const zval *value;
  const char *tainted_at_file;
  const zend_op *tainted_at;
  //zend_op_array *stack_frame;
  taint_type_t type;
  void *taint;
  // uint nothing[0x10];
} taint_variable_t;

void init_taint_tracker();

void destroy_taint_tracker();

//taint_variable_t *create_taint_variable(zend_op_array *op_array, const zend_op *tainted_op,
//                                        taint_type_t type, void *taint);
taint_variable_t *create_taint_variable(const char *file_path, const zend_op *tainted_at,
                                        taint_type_t type, void *taint);

void taint_var_add(application_t *app, const zval *taintee, taint_variable_t *taint);

void propagate_args_to_result(application_t *app, zend_execute_data *execute_data,
                              const zend_op *op, const zend_op **args, uint arg_count,
                              const char *builtin_name);

taint_variable_t *taint_var_get(const zval *value);

taint_variable_t *taint_var_get_arg(zend_execute_data *execute_data, const zend_op *arg);

taint_variable_t *taint_var_remove(const zval *value);

void taint_var_free(const zval *value);

bool propagate_zval_taint(application_t *app, zend_execute_data *execute_data,
                          zend_op_array *stack_frame, const zend_op *op, bool clobber,
                          const zval *src, const char *src_name,
                          const zval *dst, const char *dst_name);

void propagate_taint(application_t *app, zend_execute_data *execute_data,
                     zend_op_array *stack_frame, const zend_op *op);

void taint_prepare_call(application_t *app, zend_execute_data *execute_data,
                        const zend_op **args, uint arg_count);

void taint_propagate_into_arg_receivers(application_t *app, zend_execute_data *execute_data,
                                        zend_op_array *stack_frame, zend_op *op);

void taint_propagate_return(application_t *app, zend_execute_data *execute_data,
                            zend_op_array *stack_frame, const zend_op *call_op);

void taint_clear();
#endif

