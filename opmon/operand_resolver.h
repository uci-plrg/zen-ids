#ifndef _OPERAND_RESOLVER_H_
#define _OPERAND_RESOLVER_H_ 1

#include "cfg.h"

typedef enum _sink_type_t {
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
} sink_type_t;

typedef enum _source_type_t {
  SOURCE_TYPE_HTTP,      /* HTTP parameters and cookies */
  SOURCE_TYPE_SESSION,   /* session variables */
  SOURCE_TYPE_SQL,       /* database query results */
  SOURCE_TYPE_FILE,      /* file access such as fread() */
} source_type_t;

typedef union _sink_identifier_t {
  const char *call_target;
} sink_identifier_t;

typedef enum _dataflow_condition_t {
  DATAFLOW_CONDITION_DIRECT,    /* the dataflow always occurs at this op */
  DATAFLOW_CONDITION_LOGICAL,   /* the dataflow depends on an immediate condition */
  DATAFLOW_CONDITION_INDIRECT,  /* the global state affects the dataflow */
} dataflow_condition_t;

/* Notes about dataflow effects:
 *
 *   - local variable assignment never depends on global state
 *   - arrays and objects:
 *     - global state may always affect an l-value
 *     - statically declared r-value can be definite (required to be deeply static?)
 *   - type-dependent operations are only certain for statically declared types
 *   - dynamic execution and function definition can only be certain for static source
 *   -
 *
 * Given complete knowledge about the inputs to any op, we an always compute its effects.
 *
 * Static compututation of some ops depends on whether the program uses indeterminate assignment:
 *   - global variables require that all global assignments use statically determinate variables
 *   - maps require that:
 *     - all map modifications use statically determinate keys and values
 *     - control flow in map operations is statically determinate
 * In general it will be rare for these ops to be statically determinate. But we may hypothesize
 * a particular value and then proceed with analysis as if no alternatives were possible.
 *
 * Other things to model:
 *
 *   - SQL:
 *     - operation type: {insert, update, delete}
 *     - source/destination table
 *     - referenced tables
 *   - functions
 *     - sources affecting the sinks (return value, globals and I/O)
 *       - args
 *       - globals
 *       - input (files, db, system)
 *     - sinks
 *       - globals
 *       - output (page, files, db, system)
 *   - object structures
 *     - identify roots
 *     - identify each object's relationship to its root(s)
 */

typedef enum _dataflow_source_t {
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
} dataflow_source_t;

typedef enum _dataflow_source_scope_t {
  /* Only the immediate operands can affect the op result. */
  DATAFLOW_SOURCE_SCOPE_IMMEDIATE,
  /* Dataflow to the op result may potentially be limited to local variables of any routine. */
  DATAFLOW_SOURCE_SCOPE_LOCAL,
  /* Global state always flows to the op result. */
  DATAFLOW_SOURCE_SCOPE_GLOBAL,
} dataflow_source_scope_t;

void init_operand_resolver();
void destroy_operand_resolver();
const char *resolve_constant_include(zend_op *op);
char *resolve_eval_body(zend_op *op);
application_t *locate_application(const char *filename /*absolute path*/);

void dump_operand(FILE *file, char index, znode_op *operand, zend_uchar type);
void dump_fcall_opcode(FILE *file, zend_op *op, const char *routine_name);
void dump_fcall_arg(FILE *file, zend_op *op, const char *routine_name);
void dump_map_assignment(FILE *file, zend_op *op, zend_op *next_op);
void dump_foreach_fetch(FILE *file, zend_op *op, zend_op *next_op);
void dump_opcode(FILE *file, zend_op *op);

void identify_sink_operands(FILE *file, zend_op *op, sink_identifier_t id);
bool is_db_source_function(const char *type, const char *name);
bool is_db_sink_function(const char *type, const char *name);
bool is_file_source_function(const char *name);
bool is_file_sink_function(const char *name);
bool is_system_source_function(const char *name);
bool is_system_sink_function(const char *name);

#endif
