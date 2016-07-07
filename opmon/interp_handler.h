#ifndef _INTERP_HANDLER_H_
#define _INTERP_HANDLER_H_ 1

int execute_opcode(zend_execute_data *execute_data, opcode_handler_t original_handler TSRMLS_DC);

#endif
