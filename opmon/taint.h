#ifndef _TAINT_H_
#define _TAINT_H_ 1

#include "php.h"
#include "cfg.h"

/*

typedef struct _cfg_opcode_id_t {
  uint routine_hash;
  uint op_index;
} cfg_opcode_id_t;

typedef enum _dataflow_operand_index_t {
  DATAFLOW_OPERAND_RESULT,
  DATAFLOW_OPERAND_VALUE = DATAFLOW_OPERAND_RESULT,
  DATAFLOW_OPERAND_1,
  DATAFLOW_OPERAND_MAP = DATAFLOW_OPERAND_1,
  DATAFLOW_OPERAND_2,
  DATAFLOW_OPERAND_KEY = DATAFLOW_OPERAND_2,
  DATAFLOW_OPERAND_SOURCE,
  DATAFLOW_OPERAND_SINK
} dataflow_operand_index_t;

typedef struct _dataflow_operand_id_t {
  cfg_opcode_id_t opcode_id;
  dataflow_operand_index_t operand_index;
} dataflow_operand_id_t;

typedef struct _dataflow_live_variable_t {
  bool is_temp;
  union {
    uint temp_id;
    const char *var_name;
  };
  dataflow_operand_id_t src;
  cfg_opcode_id_t global_id; // where the global was locally declared (mainly for logging)
} dataflow_live_variable_t;

  For static dataflow, local variables occur per stack frame and have a type { temp, var } and name.
  Currently using the memory address of zend_op_array->opcodes as a stack frame id.

  In dynamic dataflow, a variable needs 3 types: { temp, var, global } with name and frame id.
  It would be ideal to pack all these vars into one giant hashtable, so I don't need to keep
  tagging them on the stack frames or the corresponding cfm. Issues though:

   · how to hash this thing safely?
     · 4 upper bytes of the stack_frame_id are generally available
   · how to clear a stack frame from the hashtable?

  It may be easier to just put a list of variables on each stack frame, and keep a global list.
  Maybe there's no need for TAINT_VAR_GLOBAL, since those have a different scope anyway.
  A stack frame id looks like this: { 0000 7fff e865 9180 }. So the upper 4 bytes can be messed
  with, at the slight risk of collision. But removal is hard in a perfect hash scheme, because
  how do I know what variables have been tracked in a given frame? Also, I don't know when frames
  get cleared. Though hm, this is transitory per request--it gets flushed at request start anyway.
  So could I just let them accumulate and toss the bucket at the end? I dunno how far a single
  request can really go.
 */

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
  TAINT_TYPE_REQUEST_INPUT
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
  uint nothing[0x10];
} taint_variable_t;

void init_taint_tracker();

void destroy_taint_tracker();

//taint_variable_t *create_taint_variable(zend_op_array *op_array, const zend_op *tainted_op,
//                                        taint_type_t type, void *taint);
taint_variable_t *create_taint_variable(const char *file_path, const zend_op *tainted_at,
                                        taint_type_t type, void *taint);

void taint_var_add(application_t *app, const zval *taintee, taint_variable_t *taint);

taint_variable_t *taint_var_get(const zval *value);

taint_variable_t *taint_var_get_arg(zend_execute_data *execute_data, const zend_op *arg);

taint_variable_t *taint_var_remove(const zval *value);

void taint_var_free(const zval *value);

void propagate_taint(application_t *app, zend_execute_data *execute_data,
                     zend_op_array *stack_frame, zend_op *op);

void taint_prepare_call(application_t *app, zend_execute_data *execute_data,
                        zend_op **args, uint arg_count);

void taint_propagate_into_arg_receivers(application_t *app, zend_execute_data *execute_data,
                                        zend_op_array *stack_frame, zend_op *op);

void taint_propagate_return(application_t *app, zend_execute_data *execute_data,
                            zend_op_array *stack_frame, zend_op *call_op);

void taint_clear();
#endif

