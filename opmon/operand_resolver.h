#ifndef _OPERAND_RESOLVER_H_
#define _OPERAND_RESOLVER_H_ 1

#include "cfg.h"

typedef union _sink_identifier_t {
  const char *call_target;
} sink_identifier_t;

typedef enum _dataflow_condition_t {
  DATAFLOW_CONDITION_DIRECT,    /* the dataflow always occurs at this op */
  DATAFLOW_CONDITION_LOGICAL,   /* the dataflow depends on an immediate condition */
  DATAFLOW_CONDITION_INDIRECT,  /* the global state affects the dataflow */
} dataflow_condition_t;

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

void dump_operand(FILE *file, char index, zend_op_array *ops, znode_op *operand, zend_uchar type);
void dump_fcall_opcode(FILE *file, zend_op_array *ops, zend_op *op, const char *routine_name);
void dump_fcall_arg(FILE *file, zend_op_array *ops, zend_op *op, const char *routine_name);
void dump_map_assignment(FILE *file, zend_op_array *ops, zend_op *op, zend_op *next_op);
void dump_foreach_fetch(FILE *file, zend_op_array *ops, zend_op *op, zend_op *next_op);
void dump_opcode(FILE *file, zend_op_array *ops, zend_op *op);

void identify_sink_operands(FILE *file, zend_op *op, sink_identifier_t id);
bool is_db_source_function(const char *type, const char *name);
bool is_db_sink_function(const char *type, const char *name);
bool is_file_source_function(const char *name);
bool is_file_sink_function(const char *name);
bool is_system_source_function(const char *name);
bool is_system_sink_function(const char *name);

#endif
