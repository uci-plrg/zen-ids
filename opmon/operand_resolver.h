#ifndef _OPERAND_RESOLVER_H_
#define _OPERAND_RESOLVER_H_ 1

#include "cfg.h"

void init_operand_resolver();
void destroy_operand_resolver();
const char *resolve_constant_include(zend_op *op);
char *resolve_eval_body(zend_op *op);
application_t *locate_application(const char *filename /*absolute path*/);

void dump_operand(FILE *file, char index, znode_op *operand, zend_uchar type);
void dump_fcall_opcode(FILE *file, zend_op *op, const char *routine_name);
void dump_fcall_arg(FILE *file, zend_op *op, const char *routine_name);
void dump_field_assignment(FILE *file, zend_op *op, zend_op *next_op);
void dump_foreach_fetch(FILE *file, zend_op *op, zend_op *next_op);
void dump_opcode(FILE *file, zend_op *op);

#endif
