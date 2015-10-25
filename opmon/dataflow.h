#ifndef DATAFLOW_H
#define DATAFLOW_H 1

#include "cfg.h"

typedef enum _dataflow_sink_type_t {
  SINK_TYPE_VAR_NUM,          /* modifies an int or float variable */
  SINK_TYPE_VAR_BOOL,         /* modifies a bool variable */
  SINK_TYPE_VAR_STRING,       /* modifies a string */
  SINK_TYPE_VAR_COND,         /* can change a branch condition */
  SINK_TYPE_SESSION,          /* modifies a session variable */
  SINK_TYPE_OUTPUT,           /* specifies output text */
  SINK_TYPE_EDGE,             /* specifies a control flow target, e.g. call function by name */
  SINK_TYPE_SQL,              /* specifies an SQL insert or update (possibly a fragment) */
  /* Various functions like file_put_contents(), fopen(), fwrite(), etc. It might be a good idea
   * to monitor system calls in the process, though not sure how (hook?)
   */
  SINK_TYPE_FILE,
  SINK_TYPE_CODE,             /* eval() or create_function() */
  SINK_TYPE_CLASS_HIERARCHY,  /* add subclass, method, interface, trait */
  SINK_TYPE_INCLUDE,          /* include() or require() */
  SINK_TYPE_NONE,             /* no sink here */
} dataflow_sink_type_t;

typedef enum _dataflow_source_type_t {
  SOURCE_TYPE_HTTP,      /* HTTP parameters and cookies */
  SOURCE_TYPE_SESSION,   /* session variables */
  SOURCE_TYPE_SQL,       /* database query results */
  SOURCE_TYPE_FILE,      /* file access such as fread() */
  SOURCE_TYPE_SYSTEM,    /* server and environment configuration */
  SOURCE_TYPE_NONE,      /* no source here */
} dataflow_source_type_t;

typedef union _sink_identifier_t {
  const char *call_target;
} sink_identifier_t;

typedef enum _dataflow_condition_t {
  DATAFLOW_CONDITION_DIRECT,    /* the dataflow always occurs at this op */
  DATAFLOW_CONDITION_LOGICAL,   /* the dataflow depends on an immediate condition */
  DATAFLOW_CONDITION_INDIRECT,  /* the global state affects the dataflow */
} dataflow_condition_t;

typedef enum _dataflow_source_scope_t {
  /* Only the operands themselves can affect the op result. */
  DATAFLOW_SOURCE_IMMEDIATE,
  /* Only the containing routine can affect the op result. */
  DATAFLOW_SOURCE_ROUTINE,
  /* Dataflow from other routines may affect the op result. */
  DATAFLOW_SOURCE_PROGRAM,
  /* Specific hypothetical values */
  DATAFLOW_SOURCE_HYPOTHETICAL,
  /* Nothing is known (yet) about the scope of potential dataflow to the operands. */
  DATAFLOW_SOURCE_UNRESOLVED
} dataflow_source_scope_t;

/*
typedef enum _dataflow_source_scope_t {
  / * Only the immediate operands can affect the op result. * /
  DATAFLOW_SOURCE_SCOPE_IMMEDIATE,
  / * Dataflow to the op result may potentially be limited to local variables of any routine. * /
  DATAFLOW_SOURCE_SCOPE_LOCAL,
  / * Global state always flows to the op result. * /
  DATAFLOW_SOURCE_SCOPE_GLOBAL,
} dataflow_source_scope_t;
*/

typedef struct _dataflow_source_t {
  cfg_opcode_id_t id;
  dataflow_source_type_t type;
} dataflow_source_t;

typedef struct _dataflow_sink_t {
  cfg_opcode_id_t id;
  dataflow_sink_type_t type;
} dataflow_sink_t;

typedef struct _dataflow_influence_t {
  dataflow_source_t source;
  struct _dataflow_influence_t *next;
} dataflow_influence_t;

typedef struct _dataflow_var_t {
  bool is_temp;
  union {
    uint temp_id;
    const char *name;
  };
} dataflow_var_t;

typedef struct _dataflow_operand_t {
  dataflow_var_t var;
  dataflow_influence_t *influence;
} dataflow_operand_t;

typedef struct _dataflow_opcode_t {
  cfg_opcode_id_t id;
  dataflow_source_t source;
  dataflow_sink_t sink;
  dataflow_operand_t op1;
  dataflow_operand_t op2;
  dataflow_operand_t result;
} dataflow_opcode_t;

void initialize_opcode_dump(const char *path);
void dump_script_header(const char *routine_name, uint function_hash);
void dump_function_header(const char *unit_path, const char *routine_name, uint function_hash);
void dump_operand(char index, zend_op_array *ops, znode_op *operand, zend_uchar type);
void dump_fcall_opcode(zend_op_array *ops, zend_op *op, const char *routine_name);
void dump_fcall_arg(zend_op_array *ops, zend_op *op, const char *routine_name);
void dump_map_assignment(zend_op_array *ops, zend_op *op, zend_op *next_op);
void dump_foreach_fetch(zend_op_array *ops, zend_op *op, zend_op *next_op);
void dump_opcode(zend_op_array *ops, zend_op *op);
void identify_sink_operands(zend_op *op, sink_identifier_t id);

bool is_db_source_function(const char *type, const char *name);
bool is_db_sink_function(const char *type, const char *name);
bool is_file_source_function(const char *name);
bool is_file_sink_function(const char *name);
bool is_system_source_function(const char *name);
bool is_system_sink_function(const char *name);

void add_static_dataflow_include(const char *include_path);
int analyze_dataflow(zend_file_handle *file);
void destroy_dataflow_analysis();

#endif
