#ifndef _SCRIPT_CFI_UTILS_H_
#define _SCRIPT_CFI_UTILS_H_ 1

#include "php.h"
#include "../../php/ext/session/php_session.h"

// #define OPMON_DEBUG 1

#define ALWAYS_LEVEL 0
#define ERROR_LEVEL 1
#define WARN_LEVEL 2
#define MESSAGE_LEVEL 3

// #define ENABLE_SPOT 1

#ifdef ENABLE_SPOT
# define SPOT(...) fprintf(stderr, "\t> #debug# "__VA_ARGS__)
# define SPOT_DECL(x) x
#else
# define SPOT(...)
# define SPOT_DECL(x)
#endif

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

#define LOG_AT(level, ...) \
do { \
  if (OPMON_G(verbose) >= level) \
    fprintf(stderr, "\t> "__VA_ARGS__); \
} while (0)

typedef enum _cfi_mode_t {
  CFI_MODE_TRAINING = 1,
  CFI_MODE_DB       = 2,
  CFI_MODE_DGC      = 3,
  CFI_MODE_FILE     = 4,
  CFI_MODE_RIGID    = 5,
} cfi_mode_t;

#define CFI_MODE (OPMON_G(cfi_mode))
#define IS_CFI_TRAINING() (CFI_MODE == CFI_MODE_TRAINING)
#define IS_CFI_DB() (CFI_MODE == CFI_MODE_DB)
#define IS_CFI_DGC() (CFI_MODE == CFI_MODE_DGC)
#define IS_CFI_FILE() (CFI_MODE == CFI_MODE_FILE)
#define IS_CFI_RIGID() (CFI_MODE == CFI_MODE_RIGID)
#define IS_CFI_MONITOR()                                    \
  (CFI_MODE == CFI_MODE_DB || CFI_MODE == CFI_MODE_DGC ||   \
   CFI_MODE == CFI_MODE_FILE || CFI_MODE == CFI_MODE_RIGID)
#define IS_CFI_EVO() \
  (CFI_MODE == CFI_MODE_DB || CFI_MODE == CFI_MODE_DGC || CFI_MODE == CFI_MODE_FILE)
#define IS_CFI_DATA() \
  (CFI_MODE == CFI_MODE_DB || CFI_MODE == CFI_MODE_FILE)

typedef enum _request_id_synch_t {
  REQUEST_ID_SYNCH_NONE = 0,
  REQUEST_ID_SYNCH_DB   = 1,
  REQUEST_ID_SYNCH_FILE = 2,
} request_id_synch_t;

#define REQUEST_ID_SYNCH (OPMON_G(request_id_synch))
#define HAS_REQUEST_ID_SYNCH() (REQUEST_ID_SYNCH != REQUEST_ID_SYNCH_NONE)
#define IS_REQUEST_ID_SYNCH_DB() (REQUEST_ID_SYNCH == REQUEST_ID_SYNCH_DB)
#define IS_REQUEST_ID_SYNCH_FILE() (REQUEST_ID_SYNCH == REQUEST_ID_SYNCH_FILE)

#define IS_REQUEST_LOG_ENABLED() (OPMON_G(request_log_enabled != 0))
#define IS_REQUEST_EDGE_OUTPUT_ENABLED() (OPMON_G(request_edge_enabled != 0))
#define IS_OPCODE_DUMP_ENABLED() (OPMON_G(opcode_dump_enabled != 0))
#define IS_CFI_BAILOUT_ENABLED() (OPMON_G(cfi_bailout != 0))

#define TEST(match, in) (((in) & (match)) == (match))

#define MATCH_ANY_ARG_COUNT(...) (sizeof((const char *[]){__VA_ARGS__})/sizeof(const char *))
#define MATCH_ANY(match, ...) match_any((match), MATCH_ANY_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

#define Z_STRVAL_NP(zv) ((Z_TYPE_P(zv) == IS_NULL) ? NULL : Z_STRVAL_P(zv))

#define PX "0x%llx"

#define EVAL_PATH "<eval>"
#define EVAL_FUNCTION_NAME "<eval>"
#define BASE_FRAME_HASH 1
#define SYSTEM_FRAME_HASH 2
#define REQUEST_HEADER_TAG 3
#define ENTRY_POINT_OPCODE ZEND_DO_FCALL
#define ENTRY_POINT_EXTENDED_VALUE 0xff
#define USER_SESSION_KEY "__opmon_user_session"

#define UNKNOWN_CONTEXT_ID 0xffffffffU

#define ASSERT(b) do { \
  if (!(b)) PRINT("Assert failure: %s\n", #b); \
} while (0);

#define p2int(p) ((uint_ptr_t) (p))
#define int2p(p) ((byte *) (p))

typedef unsigned char bool;
typedef unsigned long long uint64;
typedef unsigned short ushort;
typedef unsigned char byte;
typedef uint64 uint_ptr_t;

typedef struct _execution_context_t {
  uint pid;
  //const zend_op *base_op;
  uint foo;
} execution_context_t;

typedef enum _user_level_t {
  USER_LEVEL_BOTTOM = 0,
  USER_LEVEL_TOP = 0x3f,
} user_level_t;

typedef struct _user_session_t {
  user_level_t user_level;
  bool active;
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

#define Z_UNWRAP_P(val) \
do { \
  if (Z_TYPE_P(val) == IS_INDIRECT) \
    val = Z_INDIRECT_P(val); \
  ZVAL_DEREF(val); \
} while (0)

#define OP_INDEX(stack_frame, op) ((uint) ((op) - (zend_op *) (stack_frame)->opcodes))

#define IS_ARG_RECEIVE(op) ((op)->opcode == ZEND_RECV || (op)->opcode == ZEND_RECV_INIT)
#define IS_FIRST_AFTER_ARGS(op) (!IS_ARG_RECEIVE(op) && IS_ARG_RECEIVE((op)-1))

typedef enum _scalloc_lifespan_t {
  ALLOC_PROCESS,
  ALLOC_REQUEST
} scalloc_lifespan_t;

#define PROCESS_ALLOC(size) scalloc(size, ALLOC_PROCESS)
#define PROCESS_NEW(type) (type *) scalloc(sizeof(type), ALLOC_PROCESS)
#define PROCESS_FREE(p) scfree_process(p)
#define REQUEST_NEW(type) (type *) scalloc(sizeof(type), ALLOC_REQUEST)
#define REQUEST_ALLOC(size) scalloc(size, ALLOC_REQUEST)

ZEND_BEGIN_MODULE_GLOBALS(opcode_monitor)
  execution_context_t execution_context;
  const char *dataset_dir;
  const char *file_evo_log_dir;
  int verbose;
  int cfi_mode;
  int request_id_synch;
  int request_log_enabled;
  int request_edge_enabled;
  int opcode_dump_enabled;
  int cfi_bailout;
ZEND_END_MODULE_GLOBALS(opcode_monitor)

ZEND_DECLARE_MODULE_GLOBALS(opcode_monitor)

#ifdef ZTS
# define OPMON_G(v) TSRMG(opcode_monitor_globals, zend_opcode_monitor_globals *, v)
#else
# define OPMON_G(v) (opcode_monitor_globals.v)
#endif

#define ROUTINE_NAME_LENGTH 256
#define CONFIG_FILENAME_LENGTH 256
#define CONFIG_PATH_LENGTH 200

#define TAINT_ALL false

void init_utils();
void destroy_utils();

uint hash_string(const char *string);
uint64 hash_addrs(void *first, void *second);
void opmon_activate_printer();
void setup_base_path(char *path, const char *category, const char *app_path);

bool is_php_session_active();
zval *php_session_set_var(zend_string *key, zval *value);

char *request_strdup(const char *src);
const char *operand_strdup(zend_execute_data *execute_data, const znode_op *operand, zend_uchar type);
const zval *get_zval(zend_execute_data *execute_data, const znode_op *operand, zend_uchar type);
const zval *get_arg_zval(zend_execute_data *execute_data, const zend_op *arg /* ZEND_SEND_* */);
char *get_resource_filename(const zval *value);

void tokenize_file(void);

void *scalloc(size_t size, scalloc_lifespan_t type);
void scfree_process(void *p);
void scfree_request();

static inline uint64 hash_addr(void *addr)
{
  return p2int(addr);
}

static inline uint hash_routine(const char *routine_name)
{
  uint hash;
  size_t len = strlen(routine_name);
  char lowercase[ROUTINE_NAME_LENGTH];

  if (len > ROUTINE_NAME_LENGTH) {
    ERROR("Routine name '%s' (%zd) exceeds limit of %d characters.\n",
          routine_name, len, ROUTINE_NAME_LENGTH);
  }

  zend_str_tolower_copy(lowercase, routine_name, len);
  hash = hash_string(lowercase);
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

static inline bool match_any(const char *match, uint count, ...)
{
  uint i;
  va_list candidates;
  bool found = false;

  va_start(candidates, count);
  for (i = 0; i < count; i++) {
    if (strcmp(match, va_arg(candidates, const char *)) == 0) {
      found = true;
      break;
    }
  }
  va_end(candidates);

  return found;
}

static inline uint squash_trailing_slash(char *str)
{
  uint len = strlen(str), last_char;

  if (len == 0)
    return 0;

  last_char = len-1;
  if (str[last_char] == '/') {
    str[last_char] = '\0';
    return last_char;
  }
  return 0;
}

static inline void opmon_copy_value_ex(zval *dst, const zval *src, zend_refcounted *gc, uint32_t t)
{
# if SIZEOF_SIZE_T == 4
  uint32_t _w2 = src->value.ww.w2;
  Z_COUNTED_P(dst) = gc;
  dst->value.ww.w2 = _w2;
  Z_TYPE_INFO_P(dst) = t;
# elif SIZEOF_SIZE_T == 8
  Z_COUNTED_P(dst) = gc;
  Z_TYPE_INFO_P(dst) = t;
# else
#  error "Unknown SIZEOF_SIZE_T"
# endif
}

static inline void opmon_copy_value(zval *dst, const zval *src)
{
  zend_refcounted *gc = Z_COUNTED_P(src);
  uint32_t t = Z_TYPE_INFO_P(src);
  opmon_copy_value_ex(dst, src, gc, t);
}

#endif
