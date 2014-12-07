#ifndef PHP_OPCODE_MONITOR_H
#define PHP_OPCODE_MONITOR_H 1

#define PHP_OPCODE_MONITOR_VERSION "1.0"
#define PHP_OPCODE_MONITOR_EXTNAME "opmon"

#define PRINT(...) fprintf(stderr, "\t> "__VA_ARGS__)
#define PX "0x%llx"

#define ASSERT(b) do { \
  if (!(b)) PRINT("Assert failure: %s", #b); \
} while (0);

typedef unsigned long long uint64;

extern zend_module_entry opcode_monitor_module_entry;
#define phpext_opcode_monitor_ptr &opcode_monitor_module_entry

PHP_FUNCTION(opcode_monitor_string);
PHP_MINIT_FUNCTION(opcode_monitor);

#endif
