#ifndef _OPERAND_RESOLVER_H_
#define _OPERAND_RESOLVER_H_ 1

#include "cfg.h"

void init_operand_resolver();
void destroy_operand_resolver();
const char *resolve_constant_include(zend_op_array *op_array, zend_op *op);
char *resolve_eval_body(zend_op_array *op_array, zend_op *op);
application_t *locate_application(const char *filename /*absolute path*/);

#endif
