#ifndef _SCRIPT_CFI_UTILS_H_
#define _SCRIPT_CFI_UTILS_H_ 1

#include "php.h"
#include "../../php/ext/session/php_session.h"

#define ERROR_LEVEL 1
#define WARN_LEVEL 2
#define MESSAGE_LEVEL 3

#define SPOT(...) fprintf(stderr, "\t> #debug# "__VA_ARGS__)

#define PERROR(...) \
do { \
  if (OPMON_G(verbose) >= ERROR_LEVEL) \
    fprintf(stderr, "\t> #error# "__VA_ARGS__); \
    perror("\t#error# "); \
    fflush(stderr); \
} while (0)

#define ERROR(...) \
do { \
  if (OPMON_G(verbose) >= ERROR_LEVEL) \
    fprintf(stderr, "\t> #error# "__VA_ARGS__); \
} while (0)

#define WARN(...) \
do { \
  if (OPMON_G(verbose) >= WARN_LEVEL) \
    fprintf(stderr, "\t> #warn# "__VA_ARGS__); \
} while (0)

#define PRINT(...) \
do { \
  if (OPMON_G(verbose) >= MESSAGE_LEVEL) \
    fprintf(stderr, "\t> "__VA_ARGS__); \
} while (0)

#define PX "0x%llx"

#define EVAL_PATH "<eval>"
#define EVAL_FUNCTION_NAME "<eval>"
#define EVAL_HASH 0x00000000U
#define USER_SESSION_KEY "__opmon_user_session"

#define UNKNOWN_CONTEXT_ID 0xffffffffU

#define ASSERT(b) do { \
  if (!(b)) PRINT("Assert failure: %s\n", #b); \
} while (0);

#define p2int(p) ((uint_ptr_t) (p))
#define int2p(p) ((byte *) (p))

typedef unsigned long long uint64;
typedef unsigned char bool;
typedef char byte;
typedef uint64 uint_ptr_t;

typedef struct _execution_context_t {
  uint pid;
  //const zend_op *base_op;
  uint foo;
} execution_context_t;

typedef enum _user_level_t {
  USER_LEVEL_COMPILER = 0x3e,
  USER_LEVEL_BOTTOM = 0x3f
} user_level_t;

typedef struct _user_session_t {
  user_level_t user_level;
} user_session_t;

enum {
  false,
  true
};

ZEND_BEGIN_MODULE_GLOBALS(opcode_monitor)
  execution_context_t execution_context;
  const char *dataset_dir;
  int verbose;
ZEND_END_MODULE_GLOBALS(opcode_monitor)

ZEND_DECLARE_MODULE_GLOBALS(opcode_monitor)

#ifdef ZTS
# define OPMON_G(v) TSRMG(opcode_monitor_globals, zend_opcode_monitor_globals *, v)
#else
# define OPMON_G(v) (opcode_monitor_globals.v)
#endif

uint hash_string(const char *string);
uint hash_addr(void *addr);
void opmon_activate_printer();
void opmon_setup_base_path(char *path, const char *category, const char *app_path);

bool is_php_session_active();
// unused
zval *php_session_lookup_var(zend_string *key);
zval *php_session_set_var(zend_string *key, zval *value);

#endif
