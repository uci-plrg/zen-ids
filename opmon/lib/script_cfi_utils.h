#ifndef _SCRIPT_CFI_UTILS_H_
#define _SCRIPT_CFI_UTILS_H_ 1

#include "php.h"
#include "../../php/ext/session/php_session.h"

#define ERROR_LEVEL 1
#define WARN_LEVEL 2
#define MESSAGE_LEVEL 3

#define SPOT(...) fprintf(stderr, "\t> #debug# "__VA_ARGS__)
#define STATUS(...) fprintf(stderr, "\t> #status# "__VA_ARGS__)

#define MON(...) \
do { \
  if (OPMON_G(verbose) >= MESSAGE_LEVEL) \
    fprintf(stderr, "\t> #monitor# "__VA_ARGS__); \
} while (0)

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
#define ENTRY_POINT_HASH 1
#define ENTRY_POINT_OPCODE ZEND_DO_FCALL
#define ENTRY_POINT_EXTENDED_VALUE 0xff
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
  USER_LEVEL_BOTTOM = 0,
  USER_LEVEL_COMPILER = 0x3f,
} user_level_t;

typedef struct _user_session_t {
  user_level_t user_level;
} user_session_t;

enum {
  false,
  true
};

#define INCREMENT_STACK(base, ptr) \
do { \
  (ptr)++; \
  if (((ptr) - (base)) >= MAX_STACK_FRAME_##base) \
    ERROR(#ptr" exceeds max stack frame!\n"); \
} while (0)

#define DECREMENT_STACK(base, ptr) \
do { \
  (ptr)--; \
  if ((ptr) <= (base)) \
    ERROR(#ptr" hit stack bottom!\n"); \
} while (0)

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
uint64 hash_addrs(void *first, void *second);
void opmon_activate_printer();
void setup_base_path(char *path, const char *category, const char *app_path);

bool is_php_session_active();
// unused
zval *php_session_lookup_var(zend_string *key);
zval *php_session_set_var(zend_string *key, zval *value);
void set_opmon_user_level(long user_level);

static inline uint64 hash_addr(void *addr)
{
  return p2int(addr);
}

static inline uint hash_routine(const char *routine_name)
{
  uint hash = hash_string(routine_name);
  return (hash & 0x7fffffff);
}

static inline bool is_eval_routine(uint routine_hash)
{
  return (routine_hash & 0x80000000) > 0;
}

static inline uint hash_eval(uint eval_id)
{
  return (eval_id | 0x80000000);
}

static inline uint get_eval_id(uint eval_hash)
{
  return (eval_hash & 0x7fffffff);
}

#endif
