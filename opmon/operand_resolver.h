#ifndef _OPERAND_RESOLVER_H_
#define _OPERAND_RESOLVER_H_ 1

const char *resolve_constant_include(zend_op *op);
char *resolve_eval_body(zend_op *op);


#endif
