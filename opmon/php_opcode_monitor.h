#ifndef PHP_OPCODE_MONITOR_H
#define PHP_OPCODE_MONITOR_H 1

#include "php.h"

#define PHP_OPCODE_MONITOR_VERSION "1.0"
#define PHP_OPCODE_MONITOR_EXTNAME "opmon"

extern zend_module_entry opcode_monitor_module_entry;
#define phpext_opcode_monitor_ptr &opcode_monitor_module_entry;

PHP_FUNCTION(opcode_monitor_string);
PHP_MINIT_FUNCTION(opcode_monitor);

#endif
