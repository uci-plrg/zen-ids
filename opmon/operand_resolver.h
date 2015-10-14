#ifndef _OPERAND_RESOLVER_H_
#define _OPERAND_RESOLVER_H_ 1

#include "cfg.h"

typedef enum _sink_type {
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
} sink_type;

typedef enum _source_type {
  SOURCE_TYPE_HTTP,      /* HTTP parameters and cookies */
  SOURCE_TYPE_SESSION,   /* session variables */
  SOURCE_TYPE_SQL,       /* database query results */
  SOURCE_TYPE_FILE,      /* file access such as fread() */
} source_type;

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

#endif
