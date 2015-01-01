#ifndef _SCRIPT_CFI_UTILS_H_
#define _SCRIPT_CFI_UTILS_H_ 1

#define PRINT(...) fprintf(stderr, "\t> "__VA_ARGS__)
#define PX "0x%llx"

#define EVAL_PATH "<eval>"
#define EVAL_FUNCTION_NAME "<eval>"
#define EVAL_HASH 0x00000000U

#define UNKNOWN_CONTEXT_ID 0xffffffffU

#define ASSERT(b) do { \
  if (!(b)) PRINT("Assert failure: %s\n", #b); \
} while (0);

typedef struct _execution_context_t {
  //const zend_op *base_op;
  uint foo;
} execution_context_t;

ZEND_BEGIN_MODULE_GLOBALS(opcode_monitor)
  execution_context_t execution_context;
  const char *dataset_dir;
ZEND_END_MODULE_GLOBALS(opcode_monitor)

ZEND_DECLARE_MODULE_GLOBALS(opcode_monitor)

#ifdef ZTS
# define OPMON_G(v) TSRMG(opcode_monitor_globals, zend_opcode_monitor_globals *, v)
#else
# define OPMON_G(v) (opcode_monitor_globals.v)
#endif

typedef unsigned long long uint64;
typedef unsigned char bool;
typedef char byte;
typedef uint64 uint_ptr_t;

uint hash_string(const char *string);
void setup_base_path(char *path, const char *category, const char *script_path);

#endif
