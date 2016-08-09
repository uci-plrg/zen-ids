#include "php.h"
#include <pthread.h>
#include <mysql.h>
#include <openssl/md5.h>
#include <sys/file.h>
#include "SAPI.h"

#include "php_opcode_monitor.h"
#include "lib/script_cfi_utils.h"
#include "lib/script_cfi_queue.h"
#include "lib/script_cfi_hashtable.h"
#include "event_handler.h"
#include "metadata_handler.h"
#include "dataset.h"
#include "dataflow.h"
#include "taint.h"
#include "cfg_handler.h"
#include "operand_resolver.h"
#include "compile_context.h"
#include "interp_context.h"

// #define OPMON_LOG_SELECT 1
// #defne TRANSACTIONAL_SUBPROCESS 1

typedef enum _stack_motion_t {
  STACK_MOTION_RETURN = -1,
  STACK_MOTION_NONE = 0,
  STACK_MOTION_CALL = 1,
  STACK_MOTION_LEAVE = 2,
} stack_motion_t;

#define MAX_STACK_FRAME_shadow_stack 0x1000
#define MAX_STACK_FRAME_exception_stack 0x100
#define MAX_STACK_FRAME_lambda_stack 0x100
#define MAX_STACK_FRAME_fcall_stack 0x20
#define ROUTINE_NAME_LENGTH 256
#define FLUSH_MASK 0xff

typedef void (*opcode_handler_t)(void);

typedef struct _implicit_taint_t {
  uint id;
  const zend_op *end_op;
  taint_variable_t *taint;
  int last_applied_op_index;
} implicit_taint_t;

typedef struct _op_t {
  const zend_op *op;
  uint index;
} op_t;

typedef struct _op_context_t {
  zend_execute_data *execute_data;
  // zend_op *opcodes; // try to avoid this
  control_flow_metadata_t *cfm;
  op_t prev; // updated on entry to `monitor_opcode()` for call/continuation, otherwise exit
  op_t cur;  // updated on entry to `monitor_opcode()`
  bool is_unconditional_fallthrough; // set similarly to is_call_continuation
  bool is_call_continuation;
  bool is_prev_propagated;
  const char *last_builtin_name; // todo: really need a stack of these, or associate to frame
  implicit_taint_t *implicit_taint;
#ifdef TRANSACTIONAL_SUBPROCESS
  zval *last_builtin_arg;
  zend_string *last_builtin_original_arg;
#endif
} op_context_t;

typedef struct _exception_frame_t {
  zend_execute_data *execute_data;
  // zend_op *opcodes; // try to avoid this
  op_t op;
  control_flow_metadata_t *cfm;
} exception_frame_t;

typedef struct _lambda_frame_t {
  const char *name;
} lambda_frame_t;

typedef enum _pending_cfg_patch_type_t {
  PENDING_ROUTINE_EDGE,
  PENDING_OPCODE_EDGE,
  PENDING_NODE_USER_LEVEL,
} pending_cfg_patch_type_t;

typedef struct _evo_taint_t evo_taint_t;

typedef struct _pending_cfg_patch_t {
  pending_cfg_patch_type_t type;
  evo_taint_t *taint_source;
  union {
    cfg_t *cfg;                   /* PENDING_ROUTINE_EDGE */
    application_t *app;           /* PENDING_ROUTINE_EDGE, PENDING_OPCODE_EDGE */
  };
  union {
    cfg_opcode_t *cfg_opcode;     /* PENDING_NODE_USER_LEVEL */
    routine_cfg_t *from_routine;  /* PENDING_ROUTINE_EDGE */
    routine_cfg_t *routine;       /* PENDING_OPCODE_EDGE */
  };
  routine_cfg_t *to_routine;
  uint from_index;
  uint to_index;
  user_level_t user_level;
  struct _pending_cfg_patch_t *next_pending;
} pending_cfg_patch_t;

struct _evo_taint_t {
  uint request_id;
  const char *table_name;   /* NULL for file taint */
  union {
    const char *column_name;
    const char *file_path;
  };
  uint64 table_key;         /* undefined for file taint */
  struct _evo_taint_t *prev;
  struct _evo_taint_t *next;
  pending_cfg_patch_t *patch_list;
};

#define GET_TAINT_VAR_EVO_SOURCE(var) \
  ((evo_taint_t *) ((site_modification_t *) (var)->taint)->source)

#define EVO_MAX_KEY_COLUMN_COUNT 2

typedef struct _evo_key_t {
  const char *table_name;
  uint column_count;
  const char *column_names[EVO_MAX_KEY_COLUMN_COUNT];
  bool is_md5;
} evo_key_t;

#define MAX_BIGINT_CHARS 32
#define MYSQL_IDENTIFIER_MAX_LENGTH 64
#define CONNECT_TO_DB(c) mysql_real_connect(c, "localhost", "wordpressuser", "$<r!p+5A3e", "wordpress", 0, NULL, 0)

#define EVO_STATE_QUERY "SELECT request_id, table_name, column_name, table_key " \
                        "FROM opmon_evolution "                                  \
                        "WHERE request_id >= ?"
#define EVO_STATE_QUERY_FIELD_COUNT 4

static MYSQL_STMT *evo_state_query_stmt = NULL;
static MYSQL_BIND bind_evo_state_request_id[1];
static MYSQL_BIND bind_evo_state_result[EVO_STATE_QUERY_FIELD_COUNT];
static unsigned long bind_evo_state_result_lengths[EVO_STATE_QUERY_FIELD_COUNT];
static my_bool bind_evo_state_result_is_nulls[EVO_STATE_QUERY_FIELD_COUNT];
static my_bool bind_evo_state_result_errors[EVO_STATE_QUERY_FIELD_COUNT];
static uint64 evo_state_request_id, evo_state_table_key;
static char evo_state_table_name[MYSQL_IDENTIFIER_MAX_LENGTH], evo_state_column_name[MYSQL_IDENTIFIER_MAX_LENGTH];
static evo_taint_t evo_update_taint_id = { 0, evo_state_table_name, { evo_state_column_name }, 0, NULL};

#define EVO_START_QUERY "SELECT count(*) FROM opmon_evolution WHERE request_id >= ?"
#define EVO_START_QUERY_FIELD_COUNT 1

static MYSQL_STMT *evo_start_query_stmt = NULL;
static MYSQL_BIND bind_evo_start_request_id[1];
static MYSQL_BIND bind_evo_start_result[EVO_START_QUERY_FIELD_COUNT];
static unsigned long bind_evo_start_result_lengths[EVO_START_QUERY_FIELD_COUNT];
static my_bool bind_evo_start_result_is_nulls[EVO_START_QUERY_FIELD_COUNT];
static my_bool bind_evo_start_result_errors[EVO_START_QUERY_FIELD_COUNT];
static uint evo_start_count;

#define EVO_TAINT_EXPIRATION 2000

#define EVO_UPDATE_REQUEST_ID "UPDATE opmon_request_sequence SET request_id = %ld"
#define EVO_UPDATE_REQUEST_ID_BUFFER_LEN sizeof(EVO_UPDATE_REQUEST_ID) + 20

#define EVO_QUERY(query) { query, sizeof(query) - 1 }

#define DEBUG_PERMALINK 1

#ifdef DEBUG_PERMALINK
static bool is_post_permalink = false;
static bool is_homepage_request;
#endif

typedef enum _routine_edge_status_t {
  ROUTINE_EDGE_VERIFIED       = 1,
  ROUTINE_EDGE_NEW_IN_REQUEST = 2
} routine_edge_status_t;

/* hack, ought to load these from somewhere */
#define EVO_KEY_COUNT 11
static evo_key_t evo_keys[] = {
  { "wp_commentmeta", 1, { "meta_id", NULL }, false },
  { "wp_commentmeta", 1, { "meta_id", NULL }, false },
  { "wp_comments", 1, { "comment_ID", NULL }, false },
  { "wp_links", 1, { "link_id", NULL }, false },
  { "wp_options", 1, { "option_name", NULL }, true },
  { "wp_postmeta", 2, { "post_id", "meta_key" }, true },
  { "wp_posts", 1, { "ID", NULL }, false },
  { "wp_term_taxonomy", 1, { "term_taxonomy_id", NULL }, false },
  { "wp_terms", 1, { "term_id", NULL }, false },
  { "wp_usermeta", 1, { "umeta_id", NULL }, false },
  { "wp_users", 1, { "ID", NULL }, false },
  { "wp_rg_form", 1, { "id", NULL }, false },
  { "wp_rg_form_meta", 1, { "form_id", NULL }, false },
  { "wp_rg_form_view", 1, { "form_id", NULL }, false },
};

typedef struct _evo_query_t {
  const char *query;
  size_t len;
} evo_query_t;

static MYSQL *db_connection = NULL;

static const evo_query_t command_insert = EVO_QUERY("insert");
static const evo_query_t command_update = EVO_QUERY("update");
static const evo_query_t command_replace = EVO_QUERY("replace");
static const evo_query_t command_call = EVO_QUERY("call");
static const evo_query_t command_last = EVO_QUERY("");

static const evo_query_t *db_write_commands[] = {
  &command_insert,
  &command_update,
  &command_replace,
  &command_call,
  &command_last
};
#define DB_WRITE_COMMAND_COUNT ((sizeof(db_write_commands) / sizeof(evo_query_t *)) - 1)

static uint64 evo_last_synch_request_id = 0ULL; /* trimmed to taint expiration window on load */

static exception_frame_t exception_stack[MAX_STACK_FRAME_exception_stack];
static exception_frame_t *exception_frame;

//static lambda_frame_t lambda_stack[MAX_STACK_FRAME_lambda_stack];
//static lambda_frame_t *lambda_frame;

// static stack_frame_t void_frame;
static op_context_t op_context;

static application_t *current_app = NULL;

static int monitor_all_stack_motion = STACK_MOTION_CALL;

static user_session_t current_session;

static uint64 current_request_id = 0;
static uint64 evo_last_unchecked_request_id = 0;

#define DB_REQUEST_ID_SYNCH_INTERVAL 50

static sctable_t builtin_cfgs;

#define ORIGINAL_HANDLER_BITS 0x7fffffffffff
#define ROUTINE_EDGE_INDEX_CACHED 0x800000000000
#define ROUTINE_EDGE_INDEX_BITS 0xffff000000000000
#define ROUTINE_EDGE_INDEX_SHIFT 0x30
#define SCRIPT_ENTRY_EDGES_INDEX 0
#define SYSTEM_ROUTINE_EDGES_INDEX 1

#ifdef OPMON_DEBUG
static pthread_t first_thread_id;
#endif

static zend_op entry_op;

static zend_execute_data *trace_start_frame = NULL;
static bool trace_all_opcodes = false;

static bool request_had_admin = false;
static bool request_blocked = false;

typedef struct _taint_call_chain_t {
  zend_execute_data *start_frame;
  zend_execute_data *suspension_frame;
  implicit_taint_t *taint_source;
} taint_call_chain_t;

static sctable_t implicit_taint_table; // todo: allocate entries per request and clear via erase()
static implicit_taint_t pending_implicit_taint = { 0, NULL, NULL, -1 };
static uint implicit_taint_id = 0;
static taint_call_chain_t implicit_taint_call_chain = { 0 };
static scarray_t pending_cfg_patches;
static const zend_op *verified_jump = NULL;

static sctable_t evo_key_table;
static sctable_t evo_taint_table;
static scqueue_t evo_taint_queue;
static bool request_has_taint = false;
static size_t taint_log_synch_pos = 0;

#define CONTEXT_ENTRY 0xffffffffU

extern zend_dataflow_monitor_t *dataflow_hooks;
extern zend_dataflow_t *dataflow_stack_base;

static bool increment_file_synch_request_id();
static void evo_db_state_synch();
static void evo_db_state_notify();
static void evo_file_state_synch();
static void evo_commit_request_patches();
static bool evo_taint_comparator(void *a, void *b);
static void intra_monitor_opcode(zend_execute_data *execute_data, zend_op_array *op_array,
                                   const zend_op *op, int stack_motion);
static void file_read_trigger(zend_execute_data *execute_data, const char *syscall,
                              zend_op_array *op_array, const zend_op *call_op,
                              const zval *return_value, const zend_op **args, uint arg_count);
static void file_write_trigger(zend_execute_data *execute_data, const char *syscall,
                               const zend_op **args, uint arg_count);
static void propagate_internal_dataflow();

static void update_process_auth_state()
{
  if (!IS_CFI_TRAINING()) {
    if (current_session.user_level < 2 || !current_session.active) {
      op_context.cfm = NULL;
      set_monitor_mode(MONITOR_MODE_CALLS); // may be updated again during evo synch
    } else {
      set_monitor_mode(MONITOR_MODE_NONE);
      request_had_admin = true;
    }
  }
}

static void update_user_session()
{
  if (is_php_session_active()) {
    zend_string *key = zend_string_init(USER_SESSION_KEY, sizeof(USER_SESSION_KEY) - 1, 0);
    zval *session_zval = php_get_session_var(key);
    if (session_zval == NULL || Z_TYPE_INFO_P(session_zval) != IS_LONG) {
      set_opmon_user_level(USER_LEVEL_BOTTOM);
      PRINT("<session> Session has no user level for key %s during update on pid 0x%x"
            "--assigning bottom level\n", key->val, getpid());
    } else {
      PRINT("<session> Found session user level %ld\n", Z_LVAL_P(session_zval));
      current_session.user_level = (uint) Z_LVAL_P(session_zval);
    }
    zend_string_release(key);

    current_session.active = true;

    PRINT("<session> Updated current user session to level %d\n", current_session.user_level);
  } else {
    current_session.active = false;
    // TODO: why is the session sometimes inactive??
  }

  update_process_auth_state();
}

static void prepare_evo_queries()
{
  int i, status, result;

  evo_state_query_stmt = mysql_stmt_init(db_connection);
  result = mysql_stmt_prepare(evo_state_query_stmt, EVO_STATE_QUERY, sizeof(EVO_STATE_QUERY));
  if (result == 0) {
    memset(bind_evo_state_request_id, 0, sizeof(bind_evo_state_request_id));
    bind_evo_state_request_id[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind_evo_state_request_id[0].buffer = &evo_last_synch_request_id;
    bind_evo_state_request_id[0].buffer_length = sizeof(evo_last_synch_request_id);
    bind_evo_state_request_id[0].length = NULL;
    bind_evo_state_request_id[0].is_null = 0;
    bind_evo_state_request_id[0].is_unsigned = 1;

    status = mysql_stmt_bind_param(evo_state_query_stmt, bind_evo_state_request_id);
    if (status != 0)
      ERROR("Failed to bind parameters to the evo synch statement: %d\n", status);

    memset(bind_evo_state_result, 0, sizeof(bind_evo_state_result));

    for (i = 0; i < 4; i++) {
      bind_evo_state_result[i].length = &bind_evo_state_result_lengths[i];
      bind_evo_state_result[i].is_null = &bind_evo_state_result_is_nulls[i];
      bind_evo_state_result[i].error = &bind_evo_state_result_errors[i];
    }

    bind_evo_state_result[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind_evo_state_result[0].buffer = &evo_state_request_id;
    bind_evo_state_result[0].buffer_length = sizeof(evo_state_request_id);
    bind_evo_state_result[0].is_unsigned = 1;

    bind_evo_state_result[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind_evo_state_result[1].buffer = &evo_state_table_name;
    bind_evo_state_result[1].buffer_length = MYSQL_IDENTIFIER_MAX_LENGTH;

    bind_evo_state_result[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind_evo_state_result[2].buffer = &evo_state_column_name;
    bind_evo_state_result[2].buffer_length = MYSQL_IDENTIFIER_MAX_LENGTH;

    bind_evo_state_result[3].buffer_type = MYSQL_TYPE_LONGLONG;
    bind_evo_state_result[3].buffer = &evo_state_table_key;
    bind_evo_state_result[3].buffer_length = sizeof(evo_state_request_id);
    bind_evo_state_result[3].is_unsigned = 1;
  } else {
    mysql_stmt_close(evo_state_query_stmt);
    evo_state_query_stmt = NULL;

    ERROR("Failed to prepare the evo state synch query: %d\n", result);
  }

  evo_start_query_stmt = mysql_stmt_init(db_connection);
  result = mysql_stmt_prepare(evo_start_query_stmt, EVO_START_QUERY, sizeof(EVO_START_QUERY));
  if (result == 0) {
    memset(bind_evo_start_request_id, 0, sizeof(bind_evo_start_request_id));
    bind_evo_start_request_id[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind_evo_start_request_id[0].buffer = &evo_last_unchecked_request_id;
    bind_evo_start_request_id[0].buffer_length = sizeof(evo_last_unchecked_request_id);
    bind_evo_start_request_id[0].length = NULL;
    bind_evo_start_request_id[0].is_null = 0;
    bind_evo_start_request_id[0].is_unsigned = 1;

    status = mysql_stmt_bind_param(evo_start_query_stmt, bind_evo_start_request_id);
    if (status != 0)
      ERROR("Failed to bind parameters to the evo start statement: %d\n", status);

    memset(bind_evo_start_result, 0, sizeof(bind_evo_start_result));

    bind_evo_start_result[0].length = &bind_evo_start_result_lengths[0];
    bind_evo_start_result[0].is_null = &bind_evo_start_result_is_nulls[0];
    bind_evo_start_result[0].error = &bind_evo_start_result_errors[0];

    bind_evo_start_result[0].buffer_type = MYSQL_TYPE_LONG;
    bind_evo_start_result[0].buffer = &evo_start_count;
    bind_evo_start_result[0].buffer_length = sizeof(evo_start_count);
    bind_evo_start_result[0].is_unsigned = 1;
  } else {
    mysql_stmt_close(evo_start_query_stmt);
    evo_start_query_stmt = NULL;

    ERROR("Failed to prepare the evo start query: %d\n", result);
  }
}

void initialize_interp_context()
{
  uint i;

  // memset(&void_frame, 0, sizeof(stack_frame_t));
  memset(&op_context, 0, sizeof(op_context_t));

  // fake node for entry point
  memset(&entry_op, 0, sizeof(zend_op));
  entry_op.opcode = ENTRY_POINT_OPCODE;
  entry_op.extended_value = ENTRY_POINT_EXTENDED_VALUE;

  /*
  void_frame.opcodes = &entry_op;
  void_frame.opcode = entry_op.opcode;
  void_frame.cfm->cfg = routine_cfg_new(BASE_FRAME_HASH);
  routine_cfg_assign_opcode(void_frame.cfm->cfg, ENTRY_POINT_OPCODE, // verify: assign to void_frame?
                            ENTRY_POINT_EXTENDED_VALUE, 0, 0, USER_LEVEL_TOP);
  */

  memset(exception_stack, 0, 2 * sizeof(exception_frame_t));
  exception_frame = exception_stack + 1;

  // memset(lambda_stack, 0, 2 * sizeof(lambda_frame_t));
  // lambda_frame = lambda_stack + 1;

#ifdef OPMON_DEBUG
  first_thread_id = pthread_self();
#endif

  builtin_cfgs.hash_bits = 7;
  sctable_init(&builtin_cfgs);

  current_session.user_level = USER_LEVEL_BOTTOM;
  current_session.active = false;

  if (IS_CFI_DB()) {
    evo_key_table.hash_bits = 6;
    sctable_init(&evo_key_table);

    for (i = 0; i < EVO_KEY_COUNT; i++)
      sctable_add(&evo_key_table, hash_string(evo_keys[i].table_name), &evo_keys[i]);
  }

  if (IS_REQUEST_ID_SYNCH_DB()) {
    db_connection = mysql_init(NULL);
    if (CONNECT_TO_DB(db_connection) == NULL) {
      ERROR("Failed to connect to the database!\n");
    } else {
      SPOT("Successfully connected to the Opmon Evolution database.\n");

      prepare_evo_queries();
    }
  }

  if (IS_CFI_DATA()) {
    implicit_taint_table.hash_bits = 7;
    sctable_init(&implicit_taint_table);

    evo_taint_table.hash_bits = 8;
    evo_taint_table.comparator = evo_taint_comparator;
    sctable_init(&evo_taint_table);
    SCQUEUE_INIT(&evo_taint_queue, evo_taint_t, prev, next);

    scarray_init(&pending_cfg_patches);
  }
}

static control_flow_metadata_t *
initialize_entry_point(application_t *app, uint entry_point_hash, const char *routine_name)
{
  control_flow_metadata_t *entry_cfm = PROCESS_NEW(control_flow_metadata_t);

  memset(entry_cfm, 0, sizeof(control_flow_metadata_t));
  entry_cfm->cfg = routine_cfg_new(entry_point_hash);
  entry_cfm->app = app;
  entry_cfm->routine_name = routine_name;
  routine_cfg_assign_opcode(entry_cfm->cfg, ENTRY_POINT_OPCODE,
                            ENTRY_POINT_EXTENDED_VALUE, 0, 0, USER_LEVEL_TOP);

  entry_cfm->dataset = dataset_routine_lookup(app, entry_point_hash);
  if (entry_cfm->dataset == NULL)
    write_node(app, entry_point_hash, routine_cfg_get_opcode(entry_cfm->cfg, 0), 0);

  return entry_cfm;
}

static void load_entry_points(control_flow_metadata_t *entry_cfm)
{
  dataset_target_routines_t *targets;

  targets = dataset_lookup_target_routines(current_app, entry_cfm->dataset, 0);
  scarray_append(&current_app->routine_edge_targets, targets);
}

static void update_request_id()
{
  int len;
  char buffer[EVO_UPDATE_REQUEST_ID_BUFFER_LEN];

  len = snprintf(buffer, EVO_UPDATE_REQUEST_ID_BUFFER_LEN, EVO_UPDATE_REQUEST_ID,
                 current_request_id);
  if (mysql_real_query(db_connection, buffer, len) != 0) {
    ERROR("Failed to update the evo request id: %s: %s.\n",
          mysql_sqlstate(db_connection), mysql_error(db_connection));
  } else {
    evo_last_unchecked_request_id = current_request_id;
  }
}

void initialize_interp_app_context(application_t *app)
{
  current_app = app;

  app->base_frame = (void *) initialize_entry_point(app, BASE_FRAME_HASH, "<app_base_frame>");
  app->system_frame = (void *) initialize_entry_point(app, SYSTEM_FRAME_HASH, "<system_frame>");

  if (is_standalone_mode()) {
    scarray_init_ex(&current_app->routine_edge_targets, 50);
    load_entry_points((control_flow_metadata_t *) current_app->base_frame);
    load_entry_points((control_flow_metadata_t *) current_app->system_frame);

    if (current_app->evo_taint_log != NULL)
      evo_file_state_synch();
  }
}

void destroy_interp_app_context(application_t *app)
{
  routine_cfg_free(((control_flow_metadata_t *) app->base_frame)->cfg);
  PROCESS_FREE(app->base_frame);
  routine_cfg_free(((control_flow_metadata_t *) app->system_frame)->cfg);
  PROCESS_FREE(app->system_frame);

  if (IS_REQUEST_ID_SYNCH_DB()) {
    if (evo_state_query_stmt != NULL)
      mysql_stmt_close(evo_state_query_stmt);
    mysql_close(db_connection);
  }

  if (app->evo_taint_log != NULL) {
    fflush(app->evo_taint_log);
    fclose(app->evo_taint_log);
    if (app->request_id_file != NULL)
      fclose(app->request_id_file);
    if (app->evo_start_file != NULL)
      fclose(app->evo_start_file);
  }
}

uint64 interp_request_boundary(bool is_request_start)
{
  if (is_request_start) {
    char filename[CONFIG_FILENAME_LENGTH] = { 0 };
    bool local_request_id = (!HAS_REQUEST_ID_SYNCH() || IS_CFI_TRAINING());
    current_app = locate_application(((php_server_context_t *) SG(server_context))->r->filename);

    if (current_app->routine_edge_targets.capacity == 0) {
      scarray_init_ex(&current_app->routine_edge_targets, 1000);
      load_entry_points((control_flow_metadata_t *) current_app->base_frame);
      load_entry_points((control_flow_metadata_t *) current_app->system_frame);
    }

    if (current_app->evo_taint_log == NULL) {
      const char *mode = (current_app->dataset == NULL) ? "w+" : "a+";

      snprintf(filename, CONFIG_FILENAME_LENGTH,
               "%s/%s.evo", OPMON_G(file_evo_log_dir), current_app->name);
      current_app->evo_taint_log = fopen(filename, mode);
      if (current_app->evo_taint_log == NULL) {
        ERROR("Failed to open the evo taint file %s!\n", filename);
        perror(NULL);
      }
    }

    if (IS_REQUEST_ID_SYNCH_FILE() || IS_REQUEST_ID_SYNCH_DB()) {
      if (current_app->request_id_file == NULL) {
        snprintf(filename, CONFIG_FILENAME_LENGTH,
                 "%s/%s.rid", OPMON_G(file_evo_log_dir), current_app->name);
        current_app->request_id_file = fopen(filename, "r+");
        if (current_app->request_id_file == NULL)
          current_app->request_id_file = fopen(filename, "w+");
        if (current_app->request_id_file == NULL) {
          ERROR("Failed to open the request id file %s!\n", filename);
          perror(NULL);
        }
      }

      if (!increment_file_synch_request_id())
        local_request_id = true;
    }

    if (IS_CFI_DATA()) {
      if (current_app->evo_start_file == NULL) {
        snprintf(filename, CONFIG_FILENAME_LENGTH,
                 "%s/%s.eid", OPMON_G(file_evo_log_dir), current_app->name);
        current_app->evo_start_file = fopen(filename, "r+");
        if (current_app->evo_start_file == NULL)
          current_app->evo_start_file = fopen(filename, "w+");
        if (current_app->evo_start_file == NULL) {
          ERROR("Failed to open the evo start file %s!\n", filename);
          perror(NULL);
        }
      }
    }

    if (local_request_id)
      current_request_id++;

    reset_dataflow_stack();

    update_user_session();

#ifdef DEBUG_PERMALINK
    if (current_session.user_level < 2 &&
        strcmp("GET / HTTP/1.1", ((php_server_context_t *) SG(server_context))->r->the_request) == 0)
      is_homepage_request = true;
    else
      is_homepage_request = false;
#endif
  } else {
    if (IS_CFI_DB() && request_had_admin) //  && !request_has_taint)
      evo_db_state_notify();

    request_blocked = false;
    request_had_admin = false;
    flush_all_outputs(current_app);
    fflush(current_app->evo_taint_log);
  }

  if (IS_CFI_DATA()) {
    if (is_request_start) {
      if (IS_REQUEST_ID_SYNCH_DB()) {
        if (current_request_id % DB_REQUEST_ID_SYNCH_INTERVAL == 0)
          update_request_id();
        else if ((current_request_id - evo_last_unchecked_request_id) > DB_REQUEST_ID_SYNCH_INTERVAL)
          evo_last_unchecked_request_id = current_request_id; // this is really just for starting a PHP instance

        if (current_session.user_level < 2)
          evo_db_state_synch();
      } else {
        if (current_session.user_level < 2)
          evo_file_state_synch();
      }
    } else {
      sctable_clear(&implicit_taint_table);
      implicit_taint_id = 0;
      implicit_taint_call_chain.start_frame = NULL;
      implicit_taint_call_chain.suspension_frame = NULL;

      if (!request_blocked)
        evo_commit_request_patches();
      pending_cfg_patches.size = 0; /* evo: need to free them if request blocked */
    }
  }

  return current_request_id;
}

user_level_t get_current_user_level()
{
  return current_session.user_level;
}

void set_opmon_user_level(long user_level)
{
  current_session.user_level = user_level;

  if (is_php_session_active()) {
    zend_string *key = zend_string_init(USER_SESSION_KEY, sizeof(USER_SESSION_KEY) - 1, 0);
    zval *session_zval = php_get_session_var(key);
    if (session_zval == NULL || Z_TYPE_INFO_P(session_zval) != IS_LONG) {
      zval new_session_zval;
      ZVAL_LONG(&new_session_zval, user_level);
      session_zval = php_session_set_var(key, &new_session_zval);
      PRINT("<session> No user session during set_user_level--"
            "created new session user with level %ld\n", user_level);
    } else {
      PRINT("<session> Found session user level %ld during set_user_level\n",
            Z_LVAL_P(session_zval));
      Z_LVAL_P(session_zval) = user_level;
    }
    zend_string_release(key);
    PRINT("<session> Set session user with level %ld on pid 0x%x\n",
          Z_LVAL_P(session_zval), getpid());
  } else {
    ERROR("<session> User level assigned with no active PHP session!\n");
  }

  update_process_auth_state();
}

static void push_exception_frame(zend_op_array *op_array)
{
  INCREMENT_STACK(exception_stack, exception_frame);

  exception_frame->execute_data = op_context.execute_data;
  // exception_frame.opcodes = op_context.execute_data->opcodes;
  exception_frame->op.op = EG(opline_before_exception);
  exception_frame->op.index = (exception_frame->op.op - op_array->opcodes);
  exception_frame->cfm = op_context.cfm;
}

static void block_request(zend_execute_data *from_execute_data, control_flow_metadata_t *from_cfm,
                          op_t *from_op, control_flow_metadata_t *to_cfm, uint to_index)
{
  const char *address = NULL;
  bool block_now = !is_standalone_mode() && !request_blocked;

  if (block_now) {
    request_blocked = true;
    address = get_current_request_address();

    plog(current_app, PLOG_TYPE_CFG_BLOCK, "block request %08lld 0x%llx: %s\n",
         current_request_id, get_current_request_start_time(), address);
  }

  if (from_execute_data == NULL) {
    plog(current_app, PLOG_TYPE_CFG, "%s unverified: %s -> %s\n", (to_index == 0) ? "call" : "throw",
         (from_cfm == NULL) ? "<system>" : from_cfm->routine_name,
         (to_cfm == NULL) ? "?" : to_cfm->routine_name);
  } else if (from_op == NULL || from_cfm == NULL || to_cfm == NULL) {
    zend_op_array *op_array = &from_execute_data->func->op_array;

    plog(current_app, PLOG_TYPE_CFG, "%s unverified: %s:%s:%d -> %s\n",
         (to_index == 0) ? "call" : "throw", op_array->filename->val,
         (op_array->function_name == NULL) ? "<script-body>" : op_array->function_name->val,
         from_execute_data->opline->lineno, (to_cfm == NULL) ? "?" : to_cfm->routine_name);
  } else {
    if (to_index == 0) {
      plog(current_app, PLOG_TYPE_CFG, "call unverified: %04d(L%04d) %s -> %s\n",
           from_op->index, from_op->op->lineno, from_cfm->routine_name, to_cfm->routine_name);
    } else {
      plog(current_app, PLOG_TYPE_CFG, "throw unverified: %04d(L%04d) %s -> %s\n",
           from_op->index, from_op->op->lineno, from_op->op->lineno, from_cfm->routine_name,
           to_cfm->routine_name);
    }
  }

#ifdef OPMON_DEBUG
  if (from_execute_data != NULL)
    plog_stacktrace(current_app, PLOG_TYPE_CFG_DETAIL, from_execute_data);
#endif

  if (block_now && IS_CFI_BAILOUT_ENABLED()) {
    zend_error(E_CFI_CONSTRAINT, "block request %08lld 0x%llx: %s\n",
               current_request_id, get_current_request_start_time(), address);
    zend_bailout();
    ERROR("Failed to bail out on blocked request!\n");
  }
}

static void
evaluate_routine_edge(zend_execute_data *from_execute_data, control_flow_metadata_t *from_cfm,
                      op_t *from_op, control_flow_metadata_t *to_cfm, uint to_index)
{
  bool verified = false, add = false;

  if (current_session.user_level >= 2) {
    if (IS_REQUEST_EDGE_OUTPUT_ENABLED()) {
      write_request_edge(false, current_app, from_cfm->cfg->routine_hash, from_op->index,
                         to_cfm->cfg->routine_hash, to_index, current_session.user_level);
    }
    return;
  }

  if (from_cfm->dataset != NULL) {
    if (dataset_verify_routine_edge(current_app, from_cfm->dataset, from_op->index, to_index,
                                    to_cfm->cfg->routine_hash, current_session.user_level)) {
      verified = true;
    }
#ifdef OPMON_DEBUG
    else {
      dataset_verify_routine_edge(current_app, from_cfm->dataset, from_op->index, to_index,
                                  to_cfm->cfg->routine_hash, current_session.user_level);
    }
#endif
  }

  if (IS_CFI_DGC()) {
    if (verified) {
      if (implicit_taint_call_chain.start_frame != NULL &&
          implicit_taint_call_chain.suspension_frame == NULL) {
        implicit_taint_call_chain.suspension_frame = from_execute_data;

        plog(current_app, PLOG_TYPE_CFG_DETAIL,
             "call chain would be suspended at %04d(L%04d) %s -> %s\n", from_op->index,
             from_op->op->lineno, from_cfm->routine_name, to_cfm->routine_name);
#ifdef OPMON_DEBUG
        plog_stacktrace(current_app, PLOG_TYPE_CFG_DETAIL, from_execute_data);
#endif
      }
    } else {
      if (implicit_taint_call_chain.start_frame == NULL) {
        if (to_cfm->dataset == NULL &&
            (strstr(to_cfm->routine_name, "cache/") == to_cfm->routine_name ||
             strstr(to_cfm->routine_name, "__TwigTemplate") == to_cfm->routine_name)) {
          implicit_taint_call_chain.start_frame = from_execute_data;
          verified_jump = NULL;
          verified = true;

          plog(current_app, PLOG_TYPE_CFG, "call chain activated at %04d(L%04d) %s -> %s\n",
               from_op->index, from_op->op->lineno, from_cfm->routine_name, to_cfm->routine_name);
        }
      } else if (implicit_taint_call_chain.suspension_frame == NULL) {
        verified = true;

        plog(current_app, PLOG_TYPE_CFG_DETAIL, "call chain allows %04d(L%04d) %s -> %s\n",
             from_op->index, from_op->op->lineno, from_cfm->routine_name, to_cfm->routine_name);
#ifdef OPMON_DEBUG
        plog_stacktrace(current_app, PLOG_TYPE_CFG_DETAIL, from_execute_data);
#endif
      }
    }
  }

  if (IS_CFI_DATA() && !verified) {
    verified = cfg_has_routine_edge(current_app->cfg, from_cfm->cfg, from_op->index,
                                    to_cfm->cfg, to_index, current_session.user_level);
    if (verified && !IS_REQUEST_EDGE_OUTPUT_ENABLED())
      return;
  }

  if (IS_CFI_DATA() && !verified) {
    if (to_index == 0) {
      if (implicit_taint_call_chain.start_frame == NULL) {
        if (op_context.implicit_taint != NULL) {
          implicit_taint_call_chain.start_frame = from_execute_data;
          implicit_taint_call_chain.taint_source = op_context.implicit_taint;
          add = true;

          plog(current_app, PLOG_TYPE_CFG, "call chain activated at %04d(L%04d) %s -> %s\n",
               from_op->index, from_op->op->lineno,
               from_cfm->routine_name, to_cfm->routine_name);
        }
      } else {
        add = true;

        if (implicit_taint_call_chain.start_frame == op_context.execute_data) {
          plog(current_app, PLOG_TYPE_CFG, "evo span allows %04d(L%04d) %s -> %s\n",
               from_op->index, from_op->op->lineno,
               from_cfm->routine_name, to_cfm->routine_name);
        } else {
          plog(current_app, PLOG_TYPE_CFG, "call chain allows %04d(L%04d) %s -> %s\n",
               from_op->index, from_op->op->lineno,
               from_cfm->routine_name, to_cfm->routine_name);
        }
      }
    } else {
      ERROR("Dataset evolution does not support Exception edges.\n");
    }
  }

  if (write_request_edge(add, current_app, from_cfm->cfg->routine_hash, from_op->index,
                         to_cfm->cfg->routine_hash, to_index, current_session.user_level)) {
    if (!verified) {
      if (add) {
        pending_cfg_patch_t *patch = PROCESS_NEW(pending_cfg_patch_t);
        patch->type = PENDING_ROUTINE_EDGE;
        patch->taint_source = GET_TAINT_VAR_EVO_SOURCE(implicit_taint_call_chain.taint_source->taint);
        patch->app = current_app;
        patch->from_routine = from_cfm->cfg;
        patch->to_routine = to_cfm->cfg;
        patch->from_index = from_op->index;
        patch->to_index = to_index;
        patch->user_level = current_session.user_level;
        patch->next_pending = NULL;
        scarray_append(&pending_cfg_patches, patch);

        plog(current_app, PLOG_TYPE_CFG, "add (pending) taint-verified: %04d(L%04d) %s -> %s\n",
             from_op->index, from_op->op->lineno, from_cfm->routine_name, to_cfm->routine_name);
      } else {
        block_request(from_execute_data, from_cfm, from_op, to_cfm, to_index);
      }
    }
  }
}

static bool generate_routine_edge(control_flow_metadata_t *from_cfm, uint from_index,
                                  routine_cfg_t *to_cfg, uint to_index)
{
  bool add_routine_edge = !cfg_has_routine_edge(current_app->cfg, from_cfm->cfg, from_index,
                                                to_cfg, to_index, current_session.user_level);

  if (add_routine_edge) {
    cfg_add_routine_edge(current_app->cfg, from_cfm->cfg, from_index, to_cfg, to_index,
                         current_session.user_level);
    write_routine_edge(current_app, from_cfm->cfg->routine_hash, from_index,
                       to_cfg->routine_hash, to_index, current_session.user_level);
  }

  write_request_edge(add_routine_edge, current_app, from_cfm->cfg->routine_hash, from_index,
                     to_cfg->routine_hash, to_index, current_session.user_level);

  return add_routine_edge;
}

static void generate_opcode_edge(control_flow_metadata_t *cfm, uint from_index, uint to_index)
{
  bool write_edge = true;

  /*
  if (cfm->dataset != NULL) {
    if (dataset_verify_opcode_edge(cfm->dataset, from_index, to_index)) {
      write_edge = false;
      PRINT("<MON> Verified opcode edge [0x%x %u -> %u]\n",
            cfm->cfg->routine_hash, from_index, to_index);
    }
  }
  */

  if (write_edge) {
    WARN("<MON> New opcode edge [0x%x %u -> %u ul#%d]\n",
          cfm->cfg->routine_hash, from_index, to_index, current_session.user_level);
    write_op_edge(cfm->app, cfm->cfg->routine_hash, from_index, to_index,
                  current_session.user_level);
  }
}

static bool is_alias(zend_uchar first_opcode, zend_uchar second_opcode)
{
  switch (first_opcode) {
    case ZEND_FETCH_IS:
    case ZEND_FETCH_FUNC_ARG:
      return second_opcode == ZEND_FETCH_R;
    case ZEND_FETCH_DIM_FUNC_ARG:
      return ZEND_FETCH_DIM_R;
  }
  return false;
}

#if 0
static control_flow_metadata_t *lookup_cfm_by_name(zend_execute_data *execute_data,
                                                   zend_op_array *op_array)
{
  char routine_name[ROUTINE_NAME_LENGTH];
  const char *classname, *function_name;
  control_flow_metadata_t *monitored_cfm;

  if (op_array->function_name == NULL) {     // script-body
    classname = (strrchr(op_array->filename->val, '/') + 1); // relativize
    function_name = "<script-body>";
  } else if (op_array->scope == NULL || (op_array->fn_flags & ZEND_ACC_CLOSURE)) {
    classname = "<default>";                 // function call
    /*
    if (strcmp(op_array->function_name->val, "__lambda_func") == 0) {
      function_name = lambda_frame->name;
      DECREMENT_STACK(lambda_stack, lambda_frame);
    } else {
    */
      function_name = op_array->function_name->val;
    //}
  } else if (execute_data->This.value.obj == NULL) { // static method call
    classname = op_array->scope->name->val;
    function_name = op_array->function_name->val;
  } else {                                   // instance method call
    classname = op_array->scope->name->val;
    function_name = op_array->function_name->val;
  }

  sprintf(routine_name, "%s:%s", classname, function_name);

  PRINT("Lookup cfm by name %s\n", routine_name);

  monitored_cfm = get_cfm_by_name(routine_name);

  if (monitored_cfm == NULL) {
    /*
    char *routine_name_buffer = REQUEST_ALLOC(strlen(routine_name)+10);
    sprintf(routine_name_buffer, "<missing>%s", routine_name);
    cfm->routine_name = (const char *)routine_name_buffer;
    cfm->cfg = NULL;
    cfm->dataset = NULL;
    */
    ERROR("Failed to find opcodes for function %s\n", routine_name);
  }
  return monitored_cfm;
}

static control_flow_metadata_t *lookup_cfm(zend_execute_data *execute_data, zend_op_array *op_array)
{
  control_flow_metadata_t *monitored_cfm = get_cfm_by_opcodes_address(op_array->opcodes);
  // control_flow_metadata_t *monitored_cfm = establish_cfm(op_array);
  if (monitored_cfm == NULL) {
    ERROR("Failed to find CFM for opcodes at 0x%llx: %s:%s\n", hash_addr(op_array->opcodes),
          op_array->filename->val, op_array->function_name == NULL ? "-" : op_array->function_name->val);
    if (false)
      monitored_cfm = lookup_cfm_by_name(execute_data, op_array);
  }
  return monitored_cfm;
}
#endif

static inline void stack_step(zend_execute_data *execute_data, zend_op_array *op_array)
{
#ifdef TRANSACTIONAL_SUBPROCESS
  op_context.last_builtin_arg = NULL;
  op_context.last_builtin_original_arg = NULL;
#endif

  op_context.cfm = GET_OP_ARRAY_CFM(op_array);

#if 0
  if (op_array->type == ZEND_EVAL_CODE) { // eval or lambda
    op_context.cfm = get_last_eval_cfm();
  } else {
    /*
    if (execute_data->func->type == ZEND_INTERNAL_FUNCTION || op_array == NULL) {
      execute_data = execute_data->prev_execute_data;
      op_array = &execute_data->func->op_array;
    }
    */
    op_context.cfm = lookup_cfm(execute_data, op_array); // hash lookup
  }
#endif

  PRINT("Stack step to "PX": %s\n", p2int(op_array->opcodes), op_context.cfm->routine_name);
}

static inline void edge_executing(zend_execute_data *execute_data, zend_op_array *op_array)
{
  zend_execute_data *prev_execute_data = execute_data;
  control_flow_metadata_t *from_cfm;
  op_t prev = { 0 };

  if ((execute_data->func->common.fn_flags & ZEND_ACC_DTOR) != 0 &&
      execute_data->prev_execute_data->func == NULL &&
      execute_data->prev_execute_data->opline == NULL) {
    from_cfm = (control_flow_metadata_t *) current_app->system_frame;
    prev_execute_data = NULL;
  } else while (true) {
    prev_execute_data = prev_execute_data->prev_execute_data;

    if (prev_execute_data == NULL) {
      from_cfm = (control_flow_metadata_t *) current_app->base_frame;
      break;
    }

    if (prev_execute_data->func != NULL) {
      if (prev_execute_data->func != NULL && prev_execute_data->func == EG(autoload_func)) {
        from_cfm = (control_flow_metadata_t *) current_app->system_frame;
        prev_execute_data = NULL;
        break;
      }

      if (prev_execute_data->func->op_array.type != ZEND_INTERNAL_FUNCTION) {
        zend_op_array *prev_op_array = &prev_execute_data->func->op_array;

        // from_cfm = lookup_cfm(prev_execute_data, prev_op_array);
        from_cfm = GET_OP_ARRAY_CFM(prev_op_array);
        prev.op = prev_execute_data->opline;
        prev.index = (prev.op - prev_op_array->opcodes);
        break;
      }
    }
  }

  if (op_context.cfm->cfg != NULL) {
    if (IS_CFI_MONITOR()) {
      evaluate_routine_edge(prev_execute_data, from_cfm, &prev, op_context.cfm, 0/*routine entry*/);
    } else {
      if (generate_routine_edge(from_cfm, prev.index, op_context.cfm->cfg, 0/*routine entry*/)) {
#ifdef OPMON_DEBUG
        if (execute_data->prev_execute_data != NULL &&
            execute_data->prev_execute_data->func != NULL &&
            execute_data->prev_execute_data->func->op_array.type == ZEND_INTERNAL_FUNCTION) {
          plog(current_app, PLOG_TYPE_AD_HOC, "callback to %s from ", op_context.cfm->routine_name);
          if (execute_data->prev_execute_data->func->common.function_name != NULL &&
              (strcmp(execute_data->prev_execute_data->func->common.function_name->val, "call_user_func") == 0 ||
               strcmp(execute_data->prev_execute_data->func->common.function_name->val, "call_user_func_array") == 0)) {
            plog_append(current_app, PLOG_TYPE_AD_HOC, "call_user_func*");
          } else if (op_context.last_builtin_name == NULL) {
            if (strcasestr(op_context.cfm->routine_name, "classloader") != NULL) {
              plog_append(current_app, PLOG_TYPE_AD_HOC, "classloader");
            } else if (execute_data->prev_execute_data->func->common.function_name != NULL) {
              plog_append(current_app, PLOG_TYPE_AD_HOC, "%s() -- missing?",
                          execute_data->prev_execute_data->func->common.function_name->val);
            } else {
              plog_append(current_app, PLOG_TYPE_AD_HOC, "unknown internal function");
            }
          } else {
            plog_append(current_app, PLOG_TYPE_AD_HOC, "%s", op_context.last_builtin_name);
          }
          plog_append(current_app, PLOG_TYPE_AD_HOC, " in %s at op %04d\n",
                      from_cfm->routine_name, op_context.prev.index);
        }
#endif
      }

      PRINT("<0x%x> Routine call from %s to %s with opcodes at "PX"|"PX" and cfg "PX"\n",
            getpid(), from_cfm->routine_name, op_context.cfm->routine_name,
            p2int(execute_data), p2int(op_array->opcodes), p2int(op_context.cfm->cfg));
    }
  }

  if (trace_start_frame == execute_data) {
    trace_start_frame = NULL;
    trace_all_opcodes = false;
  }
}

static bool is_unconditional_fallthrough(const zend_op *op)
{
  switch (op->opcode) {
    case ZEND_RETURN:
    case ZEND_JMP:
    case ZEND_JMPZ:
    case ZEND_JMPZNZ:
    case ZEND_JMPZ_EX:
    case ZEND_JMPNZ_EX:
    case ZEND_FE_RESET_R:
      return false;
  }
  return true;
}

static bool is_return(zend_uchar opcode)
{
  switch(opcode) {
    case ZEND_RETURN:
    case ZEND_RETURN_BY_REF:
    case ZEND_FAST_RET:
      return true;
  }
  return false;
}

/*
static bool is_lambda_call_init(const zend_op *op)
{
  zend_op_array *op_array = &EG(current_execute_data)->func->op_array;

  if (op->opcode != ZEND_INIT_FCALL_BY_NAME || op_array->function_name == NULL)
    return false;

  return strcmp(op_array->function_name->val, "__lambda_func") == 0;
}
*/

#ifdef TAINT_REQUEST_INPUT
static request_input_type_t get_request_input_type(zend_execute_data *execute_data, const zend_op *op)
{
  switch (op->opcode) {
    case ZEND_FETCH_R:  /* fetch a superglobal */
    case ZEND_FETCH_W:
    case ZEND_FETCH_RW:
    case ZEND_FETCH_IS:
    case ZEND_FETCH_FUNC_ARG:
      if (op->op1_type == IS_CONST && op->op2_type == IS_UNUSED) {
        const char *superglobal_name = Z_STRVAL_P(EX_CONSTANT(op->op1));
        if (superglobal_name != NULL) {
          if (strcmp(superglobal_name, "_REQUEST") == 0)
            return REQUEST_INPUT_TYPE_REQUEST;
          if (strcmp(superglobal_name, "_GET") == 0)
            return REQUEST_INPUT_TYPE_GET;
          if (strcmp(superglobal_name, "_POST") == 0)
            return REQUEST_INPUT_TYPE_POST;
          if (strcmp(superglobal_name, "_COOKIE") == 0)
            return REQUEST_INPUT_TYPE_COOKIE;
          if (strcmp(superglobal_name, "_SERVER") == 0)
            return REQUEST_INPUT_TYPE_SERVER;
          if (strcmp(superglobal_name, "_FILES") == 0)
            return REQUEST_INPUT_TYPE_FILES;
        }
      } // (op->op1_type == IS_CV && op->op2_type == IS_UNUSED)=> variable var name, e.g.: $$key
      break;
  }
  return REQUEST_INPUT_TYPE_NONE;
}
#endif

static void inflate_call(zend_execute_data *execute_data,
                         zend_op_array *op_array, const zend_op *call_op,
                         const zend_op **args, uint *arg_count)
{
  bool done = false;
  uint init_count = 0;
  const zend_op *walk = (call_op - 1);

  *arg_count = 0;

  do {
    if (walk->opcode == ZEND_DO_FCALL) {
      init_count++;
      continue;
    }
    if (init_count > 0) {
      switch (walk->opcode) {
        case ZEND_NEW:
        case ZEND_INIT_FCALL:
        case ZEND_INIT_FCALL_BY_NAME:
        case ZEND_INIT_NS_FCALL_BY_NAME:
        case ZEND_INIT_METHOD_CALL:
        case ZEND_INIT_STATIC_METHOD_CALL:
        case ZEND_INIT_USER_CALL:
          init_count--;
          break;
      }
    } else {
      switch (walk->opcode) {
        case ZEND_INIT_FCALL:
        case ZEND_INIT_FCALL_BY_NAME:
        case ZEND_INIT_NS_FCALL_BY_NAME:
        case ZEND_NEW:
        case ZEND_INIT_METHOD_CALL:
        case ZEND_INIT_STATIC_METHOD_CALL:
        case ZEND_INIT_USER_CALL:
          done = true;
          break;
        case ZEND_SEND_VAL:
        case ZEND_SEND_VAL_EX:
        case ZEND_SEND_VAR:
        case ZEND_SEND_VAR_NO_REF:
        case ZEND_SEND_REF:
        case ZEND_SEND_VAR_EX:
        case ZEND_SEND_UNPACK:
        case ZEND_SEND_ARRAY:
        case ZEND_SEND_USER:
          args[(*arg_count)++] = walk;
          break;
      }
    }
  } while ((--walk >= op_array->opcodes) && !done);

  if (*arg_count > 1) {
    uint i, j, middle = *arg_count / 2;

    for (i = 0, j = (*arg_count - 1); i < middle; i++, j--) {
      walk = args[i];
      args[i] = args[j];
      args[j] = walk;
    }
  }
}

#ifdef TAINT_IO
static void plog_system_output_taint(const char *category, zend_execute_data *execute_data,
                                     zend_op_array *stack_frame, const zend_op *op,
                                     const zend_op **args, uint arg_count)
{
  uint i;
  taint_variable_t *taint;

  plog_call(current_app, category, op_context.last_builtin_name, stack_frame, op, arg_count, args);
  for (i = 0; i < arg_count; i++) {
    taint = taint_var_get_arg(execute_data, args[i]);
    if (taint != NULL) {
      plog(current_app, PLOG_TYPE_TAINT, "%s in %s(#%d): %04d(L%04d)%s\n",
           category, op_context.last_builtin_name, i,
           OP_INDEX(stack_frame, op), op->lineno, site_relative_path(current_app, stack_frame));
    }
  }
}
#endif

static void post_propagate_builtin(zend_op_array *op_array, const zend_op *op)
{
  zend_execute_data *execute_data = op_context.execute_data;
  const zend_op *args[0x20];
  uint arg_count;

  inflate_call(execute_data, op_array, op, args, &arg_count);

  if (IS_CFI_FILE() && current_session.user_level < 2 && request_has_taint) { // but don't clobber with untrusted user taint!
    const zval *return_value = get_zval(execute_data, &op->result, op->result_type);
    file_read_trigger(execute_data, op_context.last_builtin_name, op_array, op,
                      return_value, args, arg_count);
  }

  propagate_args_to_result(current_app, execute_data, op, args, arg_count,
                           op_context.last_builtin_name);

#ifdef TRANSACTIONAL_SUBPROCESS
  if (IS_CFI_DGC() && strcmp(op_context.last_builtin_name, "proc_open") == 0) {
    if (op_context.last_builtin_original_arg != NULL) {
      // zend_string *mbox_command = Z_STR_P(op_context.last_builtin_arg);
      ZVAL_STR(op_context.last_builtin_arg, op_context.last_builtin_original_arg); // put it back where I found it
      // efree(mbox_command);
    }
  }
#endif

#ifdef TAINT_IO
  if (TAINT_ALL) {
    if (is_file_sink_function(op_context.last_builtin_name))
      plog_system_output_taint("<file-output>", execute_data, op_array, op, args, arg_count);
    else if (is_file_source_function(op_context.last_builtin_name))
      plog_call(current_app, "<file-input>", op_context.last_builtin_name, op_array, op, arg_count, args);
    else if (is_db_sink_function("mysqli_", op_context.last_builtin_name))
      plog_system_output_taint("<db-output>", execute_data, op_array, op, args, arg_count);
    else if (is_db_source_function("mysqli_", op_context.last_builtin_name))
      plog_call(current_app, "<db-input>", op_context.last_builtin_name, op_array, op, arg_count, args);
  }
#endif

  op_context.last_builtin_name = NULL;
#ifdef TRANSACTIONAL_SUBPROCESS
  op_context.last_builtin_arg = NULL;
  op_context.last_builtin_original_arg = NULL;
#endif
}

static void configure_builtin_cfm(control_flow_metadata_t *cfm, const char *builtin_name)
{
  routine_cfg_t *cfg;
  char builtin_key_name[256]; /* N.B.: separate key per app, but universal builtin name and hash*/
  uint builtin_key;

  snprintf(builtin_key_name, 256, "%s:%s", current_app->name, builtin_name);
  builtin_key = hash_string(builtin_key_name);
  cfg = sctable_lookup(&builtin_cfgs, builtin_key);

  if (cfg == NULL) {
    uint builtin_hash = hash_routine(builtin_name);

    cfg = routine_cfg_new_empty(builtin_hash);
    sctable_add(&builtin_cfgs, builtin_key, cfg);

    write_routine_catalog_entry(current_app, builtin_hash, "builtin", builtin_name);
  }

  cfm->routine_name = builtin_name;
  cfm->cfg = cfg;
}

#ifdef TRANSACTIONAL_SUBPROCESS
# define MBOX "mbox -- "
# define MBOX_TAIL " | awk '/^Sandbox Root/ { exit } { print }'"
static void sandbox_subprocess()
{
  zval *zcommand = ZEND_CALL_ARG(EX(call), 1);
  const char *command = Z_STRVAL_P(zcommand);
  uint len = strlen(command) + strlen(MBOX) + strlen(MBOX_TAIL) + 1;
  char *mbox_command = REQUEST_ALLOC(len);

  op_context.last_builtin_arg = zcommand;
  op_context.last_builtin_original_arg = Z_STR_P(zcommand);
  snprintf(mbox_command, len, MBOX"%s"MBOX_TAIL, command);
  ZVAL_STR(zcommand, zend_string_init(mbox_command, len, 0));
}
#endif

static void fcall_executing(zend_execute_data *execute_data, zend_op_array *op_array, const zend_op *op)
{
  const zend_op *args[0x20];
  uint arg_count;
  const char *callee_name = EX(call)->func->common.function_name->val;

  inflate_call(execute_data, op_array, op, args, &arg_count);

  if (EX(call)->func->type == ZEND_INTERNAL_FUNCTION) {
    // hot!
    op_context.last_builtin_name = callee_name; // careful about use after free!

    if (op_context.cfm->cfg != NULL) {
      control_flow_metadata_t builtin_cfm = { 0 };
      char builtin_name[256];

      /* N.B.: create a separate builtin per app or they will go to the wrong catalog! */
      snprintf(builtin_name, 256, "builtin:%s", callee_name);
      configure_builtin_cfm(&builtin_cfm, builtin_name);

      if (IS_CFI_MONITOR()) {
        evaluate_routine_edge(op_context.execute_data, op_context.cfm, &op_context.cur,
                              &builtin_cfm, 0/*routine entry*/);
      } else {
        generate_routine_edge(op_context.cfm, op_context.cur.index,
                              builtin_cfm.cfg, 0/*routine entry*/);
      }
    }

#ifdef TRANSACTIONAL_SUBPROCESS
    if (IS_CFI_DGC() && strcmp(callee_name, "proc_open") == 0)
      sandbox_subprocess();
#endif

    if (current_session.user_level >= 2 || is_standalone_mode())
      file_write_trigger(execute_data, op_context.last_builtin_name, args, arg_count);

#ifdef PLOG_SYS_WRITE
    if (is_stateful_syscall(op_context.last_builtin_name)) {
      plog_call(execute_data, current_app, PLOG_TYPE_SYS_WRITE, callee_name, args, arg_count);
      plog_stacktrace(current_app, PLOG_TYPE_SYS_WRITE, execute_data);
    }
#endif
#ifdef PLOG_SYS_READ
    if (is_file_source_function(op_context.last_builtin_name)) {
      plog_call(execute_data, current_app, PLOG_TYPE_AD_HOC, callee_name, args, arg_count);
      plog_stacktrace(current_app, PLOG_TYPE_AD_HOC, execute_data);
    }
#endif
  } else {
    if (IS_CFI_DATA())
      taint_prepare_call(current_app, execute_data, args, arg_count);
  }

  if (trace_all_opcodes) {
    uint i;
    const zval *arg_value;

    plog_call(execute_data, current_app, PLOG_TYPE_AD_HOC, callee_name, args, arg_count);

    plog_append(current_app, PLOG_TYPE_AD_HOC, "\t     %s(", callee_name);
    for (i = 0; i < arg_count; i++) {
      arg_value = get_arg_zval(execute_data, args[i]);
      plog_append(current_app, PLOG_TYPE_AD_HOC, "0x%llx", (uint64) arg_value);
      if (i < (arg_count - 1))
        plog_append(current_app, PLOG_TYPE_AD_HOC, ", ");
    }
    plog_append(current_app, PLOG_TYPE_AD_HOC, ")\n");
  }
}

static zend_op *find_spanning_block_tail(const zend_op *cur_op, zend_op_array *op_array)
{
  zend_op *top = op_array->opcodes, *walk = &op_array->opcodes[op_context.prev.index - 1];

  while (walk > top) {
    switch (walk->opcode) {
      case ZEND_JMPZ:
      case ZEND_JMPZNZ:
      case ZEND_JMPZ_EX:
      case ZEND_JMPNZ_EX:
        if (OP_JMP_ADDR(walk, walk->op2) > cur_op) // alpha check
          return OP_JMP_ADDR(walk, walk->op2);
    }
    walk--;
  }
  return &op_array->opcodes[op_array->last - 1];
}

static void remove_implicit_taint()
{
  if (op_context.implicit_taint != NULL) {
    sctable_remove(&implicit_taint_table, hash_addr(op_context.execute_data));
    // REQUEST_FREE(op_context.implicit_taint);
    op_context.implicit_taint = NULL;
  }
}

static void create_request_input(zend_op_array *op_array, const zend_op *op, const zval *value,
                                 request_input_type_t input_type)
{
  request_input_t *input = REQUEST_NEW(request_input_t);
  taint_variable_t *taint_var;

  input->type = input_type;
  input->value = value;

  taint_var = create_taint_variable(site_relative_path(current_app, op_array),
                                    op, TAINT_TYPE_REQUEST_INPUT, input);
  plog(current_app, PLOG_TYPE_TAINT, "create request input at %04d(L%04d)%s\n",
       OP_INDEX(op_array, op), op->lineno, site_relative_path(current_app, op_array));
  taint_var_add(current_app, value, taint_var);
}

static void init_implicit_taint(zend_execute_data *execute_data, const zend_op *jump_op)
{
  const zend_op *jump_target = NULL;
  const zval *jump_predicate = NULL;

  if (op_context.implicit_taint != NULL)
    return;

  while (true) {
    switch (jump_op->opcode) {
      case ZEND_JMPZ:
      case ZEND_JMPZNZ:
        if (OP_JMP_ADDR(jump_op, jump_op->op2) > jump_op) { // alpha check
          jump_target = OP_JMP_ADDR(jump_op, jump_op->op2);
          if (jump_predicate == NULL)
            jump_predicate = get_zval(execute_data, &jump_op->op1, jump_op->op1_type);
        }
        break;
      case ZEND_JMPZ_EX:
      case ZEND_JMPNZ_EX:
        if (OP_JMP_ADDR(jump_op, jump_op->op2) > jump_op) {
          jump_op = OP_JMP_ADDR(jump_op, jump_op->op2);
          if (jump_predicate == NULL)
            jump_predicate = get_zval(execute_data, &jump_op->op1, jump_op->op1_type);
          continue;
        } /* else ignore back edges */
        break;
    }
    break;
  }

  if (jump_target != NULL) {
    taint_variable_t *taint = taint_var_get(jump_predicate);
    if (taint != NULL) {
      pending_implicit_taint.end_op = jump_target;
      pending_implicit_taint.taint = taint;
      pending_implicit_taint.last_applied_op_index = -1;
    }
  }
}

static void monitor_opcode(zend_execute_data *execute_data, const zend_op *op, int stack_motion)
{
  zend_op_array *op_array;

  if (op->opcode == ZEND_CALL_TRAMPOLINE)
    return; // meta

  if (execute_data->func != NULL && execute_data->func->type == ZEND_INTERNAL_FUNCTION) {
    ERROR("Cannot monitor an internal stack frame! Rewinding to the previous frame.\n");
    execute_data = execute_data->prev_execute_data;
  }

  op_array = &execute_data->func->op_array;
  op_context.execute_data = execute_data;
  op_context.cur.op = op;
  op_context.cur.index = (op - op_array->opcodes);

  /* check if ZEND_VM_SMART_BRANCH skipped a branch after the previous op */
  if (op_context.prev.op != NULL) {
    switch (op_context.prev.op->opcode) {
      case ZEND_CASE:
      case ZEND_INSTANCEOF:
      case ZEND_TYPE_CHECK:
      case ZEND_DEFINED:
      case ZEND_CHECK_VAR:
      case ZEND_IS_IDENTICAL:
      case ZEND_IS_NOT_IDENTICAL:
      case ZEND_IS_EQUAL:
      case ZEND_IS_NOT_EQUAL:
      case ZEND_IS_SMALLER:
      case ZEND_IS_SMALLER_OR_EQUAL:
      case ZEND_ISSET_ISEMPTY_VAR:
      case ZEND_ISSET_ISEMPTY_STATIC_PROP:
      case ZEND_ISSET_ISEMPTY_DIM_OBJ:
      case ZEND_ISSET_ISEMPTY_PROP_OBJ:
      case ZEND_ISSET_ISEMPTY_THIS: {
        const zend_op *skipped_branch = op_context.prev.op + 1;
        propagate_taint(current_app, execute_data, op_array, op_context.prev.op);
        propagate_taint(current_app, execute_data, op_array, skipped_branch);
        init_implicit_taint(execute_data, skipped_branch);
        op_context.is_unconditional_fallthrough = false;
        op_context.is_prev_propagated = true;
      }
    }
  }

  if (GET_OP_ARRAY_ROUTINE_HASH(op_array) == 0) {
    ERROR("No CFM for %s:%s\n", op_array->filename->val,
          op_array->function_name == NULL ? "<script-body>" : op_array->function_name->val);
  }

  /*
  if (op_array == NULL || op_array->opcodes == NULL || op_array->type == ZEND_INTERNAL_FUNCTION) {
    zend_execute_data *prev_execute_data = execute_data->prev_execute_data;
    if (stack_motion == STACK_MOTION_RETURN && prev_execute_data != NULL &&
        prev_execute_data->func != NULL) {
      op_array = &prev_execute_data->func->op_array;
      stack_step(prev_execute_data, op_array);
      op_context.prev.op = prev_execute_data->opline;
      op_context.prev.index = prev_execute_data->opline - op_array->opcodes; // todo: could there be a magic call from a 2-op instruction?
      op_context.is_call_continuation = true;
      op_context.is_unconditional_fallthrough = false;
    }
    return;
  }
  */

#ifdef OPMON_DEBUG
  if (pthread_self() != first_thread_id) {
    ERROR("Multiple threads are not supported (started on 0x%x, current is 0x%x)\n",
          (uint) first_thread_id, (uint) pthread_self());
    exit(1);
  }
#endif

  if (op->opcode == ZEND_HANDLE_EXCEPTION) {
    /*
    SPOT("@ Processing ZEND_HANDLE_EXCEPTION of "PX"|"PX"\n",
          p2int(execute_data), p2int(op_array->opcodes));

    SPOT("Exception thrown at op %d in opcodes "PX"|"PX" of routine 0x%x\n",
         cur_frame.op_index, p2int(execute_data), p2int(op_array->opcodes),
         op_context.cfm->cfg->routine_hash);
    */
    push_exception_frame(op_array);

    op_context.prev.op = NULL;
    op_context.is_prev_propagated = false;
    op_context.is_call_continuation = false;
    op_context.is_unconditional_fallthrough = false;
    return;
  }

  if (op->opcode == ZEND_CATCH) {
    stack_motion = STACK_MOTION_CALL;
  } else if (op_context.cfm == NULL && stack_motion == STACK_MOTION_NONE) {
    stack_motion = STACK_MOTION_LEAVE;
  }

  if (op->opcode == ZEND_CALL_TRAMPOLINE)
    return; // meta opcode--skip it: will process the destination next

  switch (stack_motion) {
    case STACK_MOTION_LEAVE:
      remove_implicit_taint();
      pending_implicit_taint.end_op = NULL;

      stack_step(execute_data, op_array);
      op_context.prev.op = op-1;
      op_context.is_prev_propagated = false;
      op_context.prev.index = op_context.cur.index-1; // todo: could there be a magic call from a 2-op instruction?
      op_context.is_call_continuation = true;
      op_context.is_unconditional_fallthrough = false;

      if (IS_CFI_DATA()) {
        op_context.implicit_taint = (implicit_taint_t *) sctable_lookup(&implicit_taint_table,
                                                                        hash_addr(execute_data));
      }

      if (IS_CFI_EVO()) {
        if ((op-1)->opcode == ZEND_DO_FCALL && op_context.last_builtin_name != NULL)
          post_propagate_builtin(op_array, op-1);

        if (execute_data == implicit_taint_call_chain.start_frame) {
          implicit_taint_call_chain.start_frame = NULL;

          plog(current_app, PLOG_TYPE_CFG, "call chain returned to %04d(L%04d)\n",
               op_context.cur.index, op->lineno);
        } else if (execute_data == implicit_taint_call_chain.suspension_frame) {
          implicit_taint_call_chain.suspension_frame = NULL;

          plog(current_app, PLOG_TYPE_CFG, "call chain resumed at %04d(L%04d)\n",
               op_context.cur.index, op->lineno);
        }
      }
      break;
    case STACK_MOTION_CALL: { // post-indicator: `op` is the call site
      stack_step(execute_data, op_array);
      if (op_context.cfm == NULL) {
        SPOT("Skipping call to non-existent routine %s:%s\n",
             op_array->filename->val, op_array->function_name->val);
      } else {
        edge_executing(execute_data, op_array);
      }

      op_context.prev.op = NULL;
      op_context.is_prev_propagated = false;
      op_context.is_call_continuation = false;
      op_context.is_unconditional_fallthrough = true;

      op_context.implicit_taint = NULL; // reset for new frame
      pending_implicit_taint.end_op = NULL;
      break;
    }
  }

  PRINT("Stack motion %d at op %d in "PX" %s:%s\n", stack_motion, (int)(op - op_array->opcodes),
        p2int(op_array->opcodes), op_array->filename->val,
        op_array->function_name == NULL ? "<script-body>" : op_array->function_name->val);
  PRINT("    op_context at op %d in %s\n", op_context.cur.index,
         op_context.cfm == NULL ? "<missing>" : op_context.cfm->routine_name);

#ifdef DEBUG_PERMALINK
  if (is_post_permalink && is_homepage_request && op_context.cfm != NULL &&
      op_context.cfm->cfg != NULL && op_context.cfm->cfg->routine_hash == 0x60abbe22)
  {
    plog(current_app, PLOG_TYPE_AD_HOC, "register_taxonomy at %04d(L%04d)%s\n",
         OP_INDEX(op_array, op), op->lineno, site_relative_path(current_app, op_array));
  }

  if (is_post_permalink && is_homepage_request && op_context.cfm != NULL && op_context.cfm->cfg != NULL) {
    if (op_context.cfm->cfg->routine_hash == 0x12fc6372 && op_context.cur.index == 8)
      SPOT("wait\n"); // in WP_Rewrite::init() where it calls `get_option('permalink_structure')`
    if (op_context.cfm->cfg->routine_hash == 0x2ed50d60 && op_context.cur.index == 42)
      SPOT("wait\n"); // in get_option() where it reads from `alloptions`
    if (op_context.cfm->cfg->routine_hash == 0x60abbe22 && (op_context.cur.index == 78 || op_context.cur.index == 88))
      SPOT("wait\n"); // in register_taxonomy() where it branches on non-empty permastruct
    if (op_context.cfm->cfg->routine_hash == 0x356d7234 && op_context.cur.index == 0)
      SPOT("wait\n"); // top of using_permalinks()
  }
#endif

  switch (op->opcode) {
    case ZEND_DO_FCALL:
      fcall_executing(execute_data, op_array, (zend_op *) op);
      //if (op_context.implicit_taint != NULL && implicit_taint_call_chain.start_frame == NULL)
      //  implicit_taint_call_chain.start_frame = execute_data;
      break;
    case ZEND_CATCH: {
      WARN("Exception at op %d of 0x%x caught at op index %d in opcodes "PX"|"PX" of 0x%x\n",
           exception_frame->op.index, exception_frame->cfm->cfg->routine_hash,
           op_context.cur.index, p2int(op_context.execute_data), p2int(&op_array->opcodes),
           op_context.cfm->cfg->routine_hash);

      if (exception_frame->execute_data == op_context.execute_data) {
        if (IS_CFI_DATA()) {
          ERROR("Dataset evolution does not support Exception edges.\n");
        } else if (IS_CFI_TRAINING()) {
          if (!routine_cfg_has_opcode_edge(op_context.cfm->cfg, exception_frame->op.index,
                                           op_context.cur.index)) {
            generate_opcode_edge(op_context.cfm, exception_frame->op.index,
                                 op_context.cur.index);
          } else {
            PRINT("(skipping existing exception edge)\n");
          }
        }
      } else {
        if (IS_CFI_MONITOR()) {
          evaluate_routine_edge(exception_frame->execute_data, exception_frame->cfm,
                                &exception_frame->op, op_context.cfm, op_context.cur.index);
        } else {
          generate_routine_edge(exception_frame->cfm, exception_frame->op.index,
                                op_context.cfm->cfg, op_context.cur.index);
        }
      }
      DECREMENT_STACK(exception_stack, exception_frame);
      // what to do with implicit taint here?
    } break;
  }

#ifdef TAINT_REQUEST_INPUT
  if (IS_CFI_DGC() && op_context.prev.op != NULL) {
    request_input_type_t input_type = get_request_input_type(execute_data, op_context.prev.op);
    if (input_type != REQUEST_INPUT_TYPE_NONE) { // && TAINT_ALL) {
      const zval *value = get_zval(execute_data, &op_context.prev.op->result, op_context.prev.op->result_type);
      if (value != NULL) {
        switch (Z_TYPE_P(value)) {
          case IS_ARRAY:
          case IS_OBJECT: {
            HashTable *t = Z_ARRVAL_P(value);
            Bucket *b;
            uint i;

            for (i = 0; i < t->nNumUsed; i++) {
              b = t->arData + i;
              if (Z_TYPE(b->val) != IS_UNDEF)
                create_request_input(op_array, op, &b->val, input_type);
            }
          } break;
          default:
            create_request_input(op_array, op, value, input_type);
          }
      }
    }
  }
#endif

  if (IS_CFI_TRAINING() || IS_CFI_DGC() || request_has_taint) /* N.B.: taint post-propagates! */
    intra_monitor_opcode(execute_data, op_array, op, stack_motion);
}

static inline void intra_monitor_opcode(zend_execute_data *execute_data, zend_op_array *op_array,
                                          const zend_op *op, int stack_motion)
{
  bool is_loopback, opcode_verified = false, opcode_edge_needs_update = false;
  taint_variable_t *taint_lowers_op_user_level = NULL;
  uint op_user_level = USER_LEVEL_TOP;
  cfg_opcode_t *expected_opcode = NULL;
  cfg_opcode_edge_t *opcode_edge = NULL;

#ifdef OPMON_DEBUG
  if (trace_all_opcodes) {
    plog(current_app, PLOG_TYPE_AD_HOC, "%04d(L%04d)%s:%s\n", OP_INDEX(op_array, op),
         op->lineno, site_relative_path(current_app, op_array), op_context.cfm->routine_name);
  } else {
    PRINT("\t@ %04d(L%04d)%s:%s\n", OP_INDEX(op_array, op), op->lineno,
          site_relative_path(current_app, op_array), op_context.cfm->routine_name);
  }
#endif

  //if (op_context.cfm->cfg->routine_hash == 0x356d7234)
  //  SPOT("hang here...\n");

  is_loopback = (op_context.prev.op != NULL && op_context.prev.op > op);

  if (IS_CFI_EVO()) {
    if (stack_motion == STACK_MOTION_CALL) {
      taint_propagate_into_arg_receivers(current_app, execute_data, op_array, (zend_op *) op);
    } else if (op_context.prev.op != NULL) {
      // todo: op_context.last_builtin_name clobbered on nested builtins (callback calling a builtin)
      PRINT("T %04d(L%04d)%s:%s\n\t", OP_INDEX(op_array, op_context.prev.op), op_context.prev.op->lineno,
            site_relative_path(current_app, op_array), op_context.cfm->routine_name);

      if (!op_context.is_prev_propagated)
        propagate_taint(current_app, execute_data, op_array, op_context.prev.op);
    }
  }

#ifdef OPMON_DEBUG
  if (!is_loopback && op_context.prev.op != NULL && op_context.prev.op->opcode == ZEND_DO_FCALL &&
      op_context.last_builtin_name != NULL && strcmp(op_context.last_builtin_name, "file_put_contents") == 0) {
    const zval *filepath;
    const zend_op *args[0x20];
    uint arg_count;

    inflate_call(execute_data, op_array, op_context.prev.op, args, &arg_count);
    filepath = get_arg_zval(execute_data, args[0]);
    plog(current_app, PLOG_TYPE_CFG, "file_put_contents(%s)\n", Z_STRVAL_P(filepath));
    plog_stacktrace(current_app, PLOG_TYPE_CFG, execute_data);
  }

  if (!current_session.active) {
    PRINT("<session> Inactive session while executing %s. User level is %d.\n",
          op_context.cfm->routine_name, current_session.user_level);
  }
#endif

  if (op_context.cfm->cfg == NULL) {
    ERROR("No cfg for opcodes at "PX"|"PX"\n", p2int(execute_data),
          p2int(execute_data->func->op_array.opcodes));
  } else {
    expected_opcode = routine_cfg_get_opcode(op_context.cfm->cfg, op_context.cur.index);
    if (op_context.cur.op->opcode != expected_opcode->opcode &&
        !is_alias(op_context.cur.op->opcode, expected_opcode->opcode)) {
      ERROR("Expected opcode %s at index %u, but found opcode %s in opcodes "
            PX" of routine %s (0x%x)\n",
            zend_get_opcode_name(expected_opcode->opcode), op_context.cur.index,
            zend_get_opcode_name(op_context.cur.op->opcode), p2int(op_array->opcodes),
            op_context.cfm->routine_name, op_context.cfm->cfg->routine_hash);
      op_user_level = USER_LEVEL_TOP;
    } else {
      if (op_context.cfm->dataset != NULL)
        op_user_level = dataset_routine_get_node_user_level(op_context.cfm->dataset, op_context.cur.index);
      if (expected_opcode->user_level < op_user_level)
        op_user_level = expected_opcode->user_level; // don't verify the op on this basis until the corresponding taint expires!
    }

#ifdef OPMON_DEBUG
    if (op_context.cur.index >= op_context.cfm->cfg->opcodes.size) {
      ERROR("attempt to execute foobar op %u in opcodes "PX"|"PX" of routine 0x%x\n",
            op_context.cur.index, p2int(execute_data), p2int(op_array->opcodes),
            op_context.cfm->cfg->routine_hash);
      op_context.prev.op = NULL;
      op_context.is_call_continuation = false;
      op_context.is_unconditional_fallthrough = false;
      return;
    }

    PRINT("@ Executing %s at index %u of 0x%x (user level %d)\n",
          zend_get_opcode_name(op_context.cur.op->opcode), op_context.cur.index,
          op_context.cfm->cfg->routine_hash, op_user_level);
#endif

    // slightly weak for returns: not checking continuation pc
    if (op_context.is_call_continuation || is_loopback/*safe w/o goto*/ ||
        op_context.is_unconditional_fallthrough || current_session.user_level >= op_user_level) {
      if (op_context.is_unconditional_fallthrough) {
        PRINT("@ Verified fall-through %u -> %u in 0x%x\n",
              op_context.prev.index, op_context.cur.index,
              op_context.cfm->cfg->routine_hash);
      } else {
        PRINT("@ Verified node %u in 0x%x\n", op_context.cur.index, op_context.cfm->cfg->routine_hash);
      }
      opcode_verified = true;
    } else {
      opcode_edge = routine_cfg_lookup_opcode_edge(op_context.cfm->cfg, op_context.prev.index,
                                                   op_context.cur.index);

      opcode_edge_needs_update = ((opcode_edge == NULL) ||
                                  (current_session.user_level < opcode_edge->user_level));
      if (!opcode_edge_needs_update) {
        PRINT("@ Verified opcode edge %u -> %u\n", op_context.prev.index, op_context.cur.index);
        /* not marking `opcode_verified` because taint is required on every pass */
      }
    }
  }

  if (IS_CFI_DATA()) {
    if (pending_implicit_taint.end_op != NULL) {
      if (!opcode_verified) {
        implicit_taint_t *implicit = REQUEST_NEW(implicit_taint_t);

        *implicit = pending_implicit_taint;
        if (op == pending_implicit_taint.end_op)
          implicit->end_op = find_spanning_block_tail(op, op_array);
        implicit->id = implicit_taint_id++;
        sctable_add(&implicit_taint_table, hash_addr(execute_data), implicit);
        implicit->last_applied_op_index = -1;
        op_context.implicit_taint = implicit;

        plog(current_app, PLOG_TYPE_TAINT, "activating I%d from %04d(L%04d)-%04d(L%04d)%s\n",
             op_context.implicit_taint->id,
             OP_INDEX(op_array, op), op->lineno, OP_INDEX(op_array, implicit->end_op),
             implicit->end_op->lineno, site_relative_path(current_app, op_array));

        taint_lowers_op_user_level = pending_implicit_taint.taint;
      }

      pending_implicit_taint.end_op = NULL;
    } else if (op_context.implicit_taint != NULL) {
      if (op < op_context.implicit_taint->end_op) {
        const zval *lValue;

        switch (op->opcode) {
          case ZEND_ASSIGN_REF:
            lValue = get_zval(execute_data, &op->result, op->result_type);
            plog(current_app, PLOG_TYPE_TAINT, "implicit %s (I%d)->(0x%llx) at %04d(L%04d)%s\n",
                 zend_get_opcode_name(op->opcode), op_context.implicit_taint->id, (uint64) lValue,
                 OP_INDEX(op_array, op), op->lineno, site_relative_path(current_app, op_array));
            taint_var_add(current_app, lValue, op_context.implicit_taint->taint);
          case ZEND_ASSIGN: /* FT */
          case ZEND_SEND_VAL:
          case ZEND_SEND_VAL_EX:
          case ZEND_SEND_VAR:
          case ZEND_SEND_VAR_NO_REF:
          case ZEND_SEND_REF:
          case ZEND_SEND_VAR_EX:
          case ZEND_SEND_UNPACK:
          case ZEND_SEND_ARRAY:
          case ZEND_SEND_USER:
            lValue = get_zval(execute_data, &op->op1, op->op1_type);
            plog(current_app, PLOG_TYPE_TAINT, "implicit %s (I%d)->(0x%llx) at %04d(L%04d)%s\n",
                 zend_get_opcode_name(op->opcode), op_context.implicit_taint->id, (uint64) lValue,
                 OP_INDEX(op_array, op), op->lineno, site_relative_path(current_app, op_array));
            taint_var_add(current_app, lValue, op_context.implicit_taint->taint);
            op_context.implicit_taint->last_applied_op_index = op_context.cur.index;
        }

#ifdef PLOG_TAINT
        plog(current_app, PLOG_TYPE_TAINT, "+implicit on %04d(L%04d)%s until %04d(L%04d)\n",
             OP_INDEX(op_array, op), op->lineno, site_relative_path(current_app, op_array),
             OP_INDEX(op_array, op_context.implicit_taint->end_op), op_context.implicit_taint->end_op->lineno);
        plog_taint_var(current_app, op_context.implicit_taint->taint, 0);
#endif

        taint_lowers_op_user_level = op_context.implicit_taint->taint;
      } else {
        if (op_context.prev.op != NULL && op_context.prev.op->opcode == ZEND_JMP &&
            op_context.prev.op < op_context.implicit_taint->end_op) {
          taint_lowers_op_user_level = op_context.implicit_taint->taint; /* extend to bottom of branch diamond */
        }
        remove_implicit_taint();
      }
    }

    init_implicit_taint(execute_data, op);

#ifdef PLOG_CFG
    if (!opcode_verified && current_session.user_level < 2) {
      plog(current_app, PLOG_TYPE_CFG, "opcode unverified %s %04d(L%04d)%s\n",
           zend_get_opcode_name(op->opcode), OP_INDEX(op_array, op), op->lineno,
           site_relative_path(current_app, op_array));
    }
#endif
  } else if (IS_CFI_DGC()) {
    if (implicit_taint_call_chain.start_frame != NULL) {
      if (opcode_verified) {
        const zend_op *jump_op = op;

        switch (jump_op->opcode) {
          case ZEND_JMPZ:
          case ZEND_JMPZNZ:
          case ZEND_JMPZ_EX:
          case ZEND_JMPNZ_EX:
            if (OP_JMP_ADDR(jump_op, jump_op->op2) > jump_op) {
              verified_jump = jump_op;
            } break;
          default:
            verified_jump = NULL;
        }
      } else {
        if (verified_jump != NULL) {
          plog(current_app, PLOG_TYPE_CFG_DETAIL,
               "jump with DGC escort into untrusted territory at %04d(L%04d)%s\n",
               OP_INDEX(op_array, verified_jump), verified_jump->lineno,
               site_relative_path(current_app, op_array));
        }
      }
    } else {
      verified_jump = NULL;
    }
  }

  if (IS_CFI_TRAINING()) {
    if (opcode_edge_needs_update) {
      if (opcode_edge == NULL) {
        routine_cfg_add_opcode_edge(op_context.cfm->cfg, op_context.prev.index, op_context.cur.index,
                                    current_session.user_level);
      } else {
        opcode_edge->user_level = current_session.user_level;
      }
      write_op_edge(current_app, op_context.cfm->cfg->routine_hash, op_context.prev.index,
                    op_context.cur.index, current_session.user_level);
    }
  } else {
    if (opcode_edge_needs_update && taint_lowers_op_user_level != NULL) {
      pending_cfg_patch_t *patch = PROCESS_NEW(pending_cfg_patch_t);
      patch->type = PENDING_OPCODE_EDGE;
      patch->taint_source = GET_TAINT_VAR_EVO_SOURCE(taint_lowers_op_user_level);
      patch->app = current_app;
      patch->routine = op_context.cfm->cfg;
      patch->from_index = op_context.prev.index;
      patch->to_index = op_context.cur.index;
      patch->user_level = current_session.user_level;
      patch->next_pending = NULL;
      scarray_append(&pending_cfg_patches, patch);

#ifdef OPMON_DEBUG
      {
        compiled_edge_target_t compiled_target;
        // todo: also allow conditional branches to have op edges
        compiled_target = get_compiled_edge_target(op_context.prev.op, op_context.prev.index);
        if (compiled_target.type != COMPILED_EDGE_DIRECT &&
            compiled_target.type != COMPILED_EDGE_INDIRECT) {
          ERROR("Generating opcode edge from compiled target type %d (opcode 0x%x)\n",
                compiled_target.type, op_context.prev.op->opcode);
        }
      }
#endif
    }

    if (taint_lowers_op_user_level != NULL) {
      pending_cfg_patch_t *patch = PROCESS_NEW(pending_cfg_patch_t);
      patch->type = PENDING_NODE_USER_LEVEL;
      patch->taint_source = GET_TAINT_VAR_EVO_SOURCE(taint_lowers_op_user_level);
      patch->cfg_opcode = expected_opcode;
      patch->user_level = current_session.user_level;
      patch->next_pending = NULL;
      scarray_append(&pending_cfg_patches, patch);
    }
  }

  /*
  if (is_lambda_call_init(op)) {
    if ((op->op2_type == IS_CV || op->op2_type == IS_VAR) &&
        *EX_VAR(op->op2.var)->value.str->val == '\0') {
      INCREMENT_STACK(lambda_stack, lambda_frame);
      lambda_frame->name = EX_VAR(op->op2.var)->value.str->val + 1;
      PRINT("@ Push call to %s\n", lambda_frame->name);
    }
  }
  */

  op_context.prev.op = op;
  op_context.prev.index = op_context.cur.index;
  op_context.is_call_continuation = false;
  op_context.is_unconditional_fallthrough = is_unconditional_fallthrough(op_context.cur.op);
  op_context.is_prev_propagated = false;
}

#define ZEND_VM_FP_GLOBAL_REG "%r14"
#define ZEND_VM_IP_GLOBAL_REG "%r15"

#pragma GCC diagnostic ignored "-Wvolatile-register-var"
register zend_execute_data* volatile execute_data __asm__(ZEND_VM_FP_GLOBAL_REG);
register const zend_op* volatile opline __asm__(ZEND_VM_IP_GLOBAL_REG);
#pragma GCC diagnostic warning "-Wvolatile-register-var"

static bool is_prev_frame(zend_execute_data *walk, zend_execute_data *potential_prev_frame)
{
  do {
    walk = walk->prev_execute_data;
    if (walk == potential_prev_frame)
      return true;
  } while (walk != NULL);

  return false;
}

static void execute_opcode_direct_patch(zend_execute_data *caller_execute_data, const zend_op *caller_opline)
{
  // ZEND_VM_LOOP_INTERRUPT_CHECK(); // alpha

  while (1) {
    uint64 original_handler_addr = p2int(opline->handler) & ORIGINAL_HANDLER_BITS;
    opcode_handler_t original_handler = (opcode_handler_t) int2p(original_handler_addr);

    original_handler();

    if (UNEXPECTED(!opline)) {
      execute_data = caller_execute_data;
      opline = caller_opline;
      return;
    }
  }
  zend_error_noreturn(E_ERROR, "Arrived at end of main loop which shouldn't happen");
}

void execute_opcode_monitor_all(zend_execute_data *cur_execute_data)
{
  const zend_op *caller_opline = opline;
  zend_execute_data *caller_execute_data = execute_data;
  execute_data = cur_execute_data;

  opline = EX(opline);

  monitor_all_stack_motion = STACK_MOTION_CALL;

  // ZEND_VM_LOOP_INTERRUPT_CHECK(); // alpha

  while (1) {
    uint64 original_handler_addr = p2int(opline->handler) & ORIGINAL_HANDLER_BITS;
    opcode_handler_t original_handler = (opcode_handler_t) int2p(original_handler_addr);

    if (execute_data != cur_execute_data) {
      if (is_prev_frame(cur_execute_data, execute_data))
        monitor_all_stack_motion = STACK_MOTION_LEAVE;
      else
        monitor_all_stack_motion = STACK_MOTION_CALL;
      cur_execute_data = execute_data;
    }

    monitor_opcode(execute_data, opline, monitor_all_stack_motion);

    monitor_all_stack_motion = STACK_MOTION_NONE; // clear early--may become a call @vm_monitor_call()

    original_handler(); // on ZEND_VM_ENTER(): `op_context.cfm = NULL`

    if (current_session.user_level >= 2) { /* last opcode elevated */
      execute_opcode_direct_patch(caller_execute_data, caller_opline);
      return;
    }

    propagate_internal_dataflow();

    if (UNEXPECTED(!opline)) {
      op_context.cfm = NULL;
      execute_data = caller_execute_data;
      opline = caller_opline;
      return;
    }
  }
  zend_error_noreturn(E_ERROR, "Arrived at end of main loop which shouldn't happen");
}

static void block_edge(zend_execute_data *from_execute_data, zend_execute_data *to_execute_data)
{
  zend_op_array *from_op_array = &from_execute_data->func->op_array;
  // control_flow_metadata_t *from_cfm = lookup_cfm(from_execute_data, from_op_array);
  control_flow_metadata_t *from_cfm = GET_OP_ARRAY_CFM(from_op_array);
  op_t from_op = {
    from_execute_data->opline,
    from_execute_data->opline - from_op_array->opcodes
  };

  zend_op_array *to_op_array = &to_execute_data->func->op_array;
  // control_flow_metadata_t *to_cfm = lookup_cfm(to_execute_data, to_op_array);
  control_flow_metadata_t *to_cfm = GET_OP_ARRAY_CFM(to_op_array);
  block_request(from_execute_data, from_cfm, &from_op, to_cfm, 0);
}

static inline void monitor_call_from_user(zend_execute_data *from_execute_data, const zend_op *opline)
{
  bool block = false;
  uint to_routine_hash;
  zend_op_array *to_op_array;
  control_flow_metadata_t *to_cfm = NULL;
  control_flow_metadata_t *from_cfm = NULL;
  dataset_target_routines_t *targets = NULL;
  uint64 routine_edges_index, routine_edge_metadata = p2int(opline->handler);

  if ((routine_edge_metadata & ROUTINE_EDGE_INDEX_CACHED) == 0) {
    uint64 original_handler_addr = p2int(opline->handler) & ORIGINAL_HANDLER_BITS;
    zend_op_array *op_array = &from_execute_data->func->op_array;
    uint from_index = opline - op_array->opcodes;

    // if (opline->opcode == ZEND_CALL_TRAMPOLINE)
    //  return;

    original_handler_addr |= ROUTINE_EDGE_INDEX_CACHED;
    from_cfm = GET_OP_ARRAY_CFM(op_array);
    // from_cfm = lookup_cfm(from_execute_data, op_array);
    if (from_cfm == NULL || from_cfm->dataset == NULL) {
      block = true;
    } else {

      targets = dataset_lookup_target_routines(current_app, from_cfm->dataset, from_index);
      // debug
      //if (targets == NULL)
      //  targets = dataset_lookup_target_routines(current_app, from_cfm->dataset, from_index);

      if (targets != NULL) {
        routine_edges_index = current_app->routine_edge_targets.size;
        original_handler_addr |= (routine_edges_index << ROUTINE_EDGE_INDEX_SHIFT);

        scarray_append(&current_app->routine_edge_targets, targets);

        PRINT("Append 0x%x:%d -> { "PX" } at index %d\n", from_cfm->cfg->routine_hash,
              from_index, p2int(targets), (int) routine_edges_index);
      } else {
        block = true;
      }
    }
    ((zend_op *) opline)->handler = (opcode_handler_t) int2p(original_handler_addr);
  } else {
    routine_edges_index = routine_edge_metadata >> ROUTINE_EDGE_INDEX_SHIFT;
    targets = current_app->routine_edge_targets.data[routine_edges_index];
    block = (targets == NULL);
  }

  if (!block) {
    to_op_array = &execute_data->func->op_array;

    to_routine_hash = GET_OP_ARRAY_ROUTINE_HASH(to_op_array);

    block = !dataset_verify_routine_target(targets, to_routine_hash, 0, current_session.user_level,
                                           to_op_array->type == ZEND_EVAL_CODE);
  } else if (targets == NULL) {
    to_op_array = &execute_data->func->op_array;
    to_cfm = GET_OP_ARRAY_CFM(to_op_array);
    // to_cfm = lookup_cfm(execute_data, to_op_array);
    SPOT_DECL( zend_op_array *from_op_array = &from_execute_data->func->op_array; )
    SPOT_DECL( uint from_index = opline - from_op_array->opcodes; )

    if (to_cfm == NULL) {
      SPOT("Skipping verification of call to missing method %s:%s() with no targets\n",
           to_op_array->filename->val, to_op_array->function_name->val);
    } else if (from_cfm == NULL) {
      SPOT("No target routines for opcodes "PX" %s:%s -> %s 0x%x\n",
           p2int(from_op_array->opcodes), from_op_array->filename->val,
           from_op_array->function_name == NULL ? "<script-body>" : from_op_array->function_name->val,
           to_cfm->routine_name, to_cfm->cfg->routine_hash);
    } else {
      SPOT("No target routines for opcodes "PX" %s 0x%x:%d -> %s 0x%x\n", p2int(from_op_array->opcodes),
           from_cfm->routine_name, from_cfm->cfg->routine_hash, from_index,
           to_cfm->routine_name, to_cfm->cfg->routine_hash);
    }
  }

  if (block)
    block_edge(from_execute_data, execute_data);
}

static void monitor_call_from_system(control_flow_metadata_t *from_cfm, uint edges_index)
{
  bool block;
  dataset_target_routines_t *targets = current_app->routine_edge_targets.data[edges_index];
  zend_op_array *to_op_array = &execute_data->func->op_array;
  control_flow_metadata_t *to_cfm = GET_OP_ARRAY_CFM(to_op_array);
  // control_flow_metadata_t *to_cfm = lookup_cfm(execute_data, to_op_array);


  if (to_cfm == NULL) {
    SPOT("Skipping verification of call from system to non-existent routine %s:%s()\n",
         to_op_array->filename->val, to_op_array->function_name->val);
    return;
  }

  if (targets == NULL) {
    block = true;
  } else {
    block = !dataset_verify_routine_target(targets, to_cfm->cfg->routine_hash, 0,
                                           current_session.user_level,
                                           to_op_array->type == ZEND_EVAL_CODE);
  }

  if (block)
    block_request(NULL, from_cfm, NULL, to_cfm, 0);
}

void vm_call_plain()
{
  execute_data = EG(current_execute_data);
  opline = EX(opline);
}

void vm_monitor_call()
{
  monitor_all_stack_motion = STACK_MOTION_CALL;

  vm_call_plain();
}

// alpha: missing try/catch edges in quick mode
void vm_monitor_call_quick()
{
  zend_execute_data *from_execute_data = execute_data;
  execute_data = EG(current_execute_data);

  monitor_call_from_user(from_execute_data, opline);

  opline = EX(opline);
}

static inline void monitor_top_entry()
{
  zend_execute_data *user_caller_execute_data = execute_data->prev_execute_data;

  while (true) {
    if (user_caller_execute_data == NULL) {
      uint edge_set = SCRIPT_ENTRY_EDGES_INDEX;

      if ((execute_data->func->common.fn_flags & ZEND_ACC_DTOR) == ZEND_ACC_DTOR)
        edge_set = SYSTEM_ROUTINE_EDGES_INDEX;

      monitor_call_from_system((control_flow_metadata_t *) current_app->base_frame, edge_set);
      return;
    }

    if (user_caller_execute_data->func != NULL &&
        user_caller_execute_data->func == EG(autoload_func)) { // seems like we could cache this to avoid the remote load
      monitor_call_from_system((control_flow_metadata_t *) current_app->system_frame,
                               SYSTEM_ROUTINE_EDGES_INDEX);
      return;
    }

    if (user_caller_execute_data->func != NULL &&
        user_caller_execute_data->func->type != ZEND_INTERNAL_FUNCTION)
    {
      monitor_call_from_user(user_caller_execute_data, user_caller_execute_data->opline);
      return;
    }

    user_caller_execute_data = user_caller_execute_data->prev_execute_data;
  }
}

void execute_opcode_monitor_calls(zend_execute_data *cur_execute_data)
{
  const zend_op *caller_opline = opline;
  zend_execute_data *caller_execute_data = execute_data;
  execute_data = cur_execute_data;

  monitor_top_entry();

  opline = EX(opline);

  // ZEND_VM_LOOP_INTERRUPT_CHECK(); // alpha

  while (1) {
    uint64 original_handler_addr = p2int(opline->handler) & ORIGINAL_HANDLER_BITS;
    opcode_handler_t original_handler = (opcode_handler_t) int2p(original_handler_addr);

    original_handler(); // ZEND_VM_ENTER() hooked to `vm_monitor_call*()`

    if (UNEXPECTED(!opline)) {
      execute_data = caller_execute_data;
      opline = caller_opline;
      return;
    }
  }
  zend_error_noreturn(E_ERROR, "Arrived at end of main loop which shouldn't happen");
}

void execute_opcode_direct(zend_execute_data *cur_execute_data)
{
  const zend_op *caller_opline = opline;
  zend_execute_data *caller_execute_data = execute_data;
  execute_data = cur_execute_data;

  opline = EX(opline);

  // ZEND_VM_LOOP_INTERRUPT_CHECK(); // alpha

  while (1) {
    uint64 original_handler_addr = p2int(opline->handler) & ORIGINAL_HANDLER_BITS;
    opcode_handler_t original_handler = (opcode_handler_t) int2p(original_handler_addr);

    original_handler();

    if (UNEXPECTED(!opline)) {
      execute_data = caller_execute_data;
      opline = caller_opline;
      return;
    }
  }
  zend_error_noreturn(E_ERROR, "Arrived at end of main loop which shouldn't happen");
}

/************* Evo Functions *************/

static inline bool is_db_taint(evo_taint_t *taint)
{
  return taint->table_name != NULL;
}

static bool evo_taint_comparator(void *a, void *b)
{
  evo_taint_t *first = (evo_taint_t *) a;
  evo_taint_t *second = (evo_taint_t *) b;

  if (is_db_taint(first) && is_db_taint(second)) {
    return (strcmp(first->table_name, second->table_name) == 0 &&
            strcmp(first->column_name, second->column_name) == 0 &&
            first->table_key == second->table_key);
  } else {
    return (strcmp(first->column_name, second->column_name) == 0);
  }
}

static inline uint hash_evo_taint(evo_taint_t *taint)
{
  /* crowd the low bits for distribution--exact match goes field by field */
  if (is_db_taint(taint))
    return hash_string(taint->table_name) ^ hash_string(taint->column_name) ^ taint->table_key;
  else
    return hash_string(taint->file_path);
}

static inline void file_read_trigger(zend_execute_data *execute_data, const char *syscall,
                                     zend_op_array *op_array, const zend_op *call_op,
                                     const zval *return_value, const zend_op **args, uint arg_count)
{
  char *src = NULL;

  if (MATCH_ANY(syscall, "fread", "fgets", "fgetss", "fgetcsv")) {
    src = get_resource_filename(get_arg_zval(execute_data, args[0]));
  } else if (MATCH_ANY(syscall,
                       "file_get_contents", "file_exists", "filectime", "filemtime")) {
    if (arg_count < 1) {
      ERROR("Missing expected arg to %s(). Cannot trigger.\n", syscall);
      return;
    }
    src = Z_STRVAL_NP(get_arg_zval(execute_data, args[0]));
  }

  if (src != NULL) {
    uint squashed = squash_trailing_slash(src);
    evo_taint_t *found = NULL, lookup = { 0, NULL, { src }, 0ULL, NULL };
    uint field_hash = hash_evo_taint(&lookup);

    found = (evo_taint_t *) sctable_lookup_value(&evo_taint_table, field_hash, &lookup);
    if (found != NULL) {
      taint_variable_t *var;
      site_modification_t *mod = REQUEST_NEW(site_modification_t);

      memset(mod, 0, sizeof(site_modification_t));
      mod->type = SITE_MOD_FILE;
      mod->file_path = request_strdup(src);
      mod->source = found;

      var = create_taint_variable(site_relative_path(current_app, op_array),
                                  call_op, TAINT_TYPE_SITE_MOD, mod);

      plog(current_app, PLOG_TYPE_TAINT, "file-read at %04d(L%04d)%s\n",
           op_context.cur.index, call_op->lineno, site_relative_path(current_app, op_array));
      taint_var_add(current_app, return_value, var);
    }

    if (squashed > 0)
      src[squashed] = '/';
  }
}

static inline void file_write_trigger(zend_execute_data *execute_data, const char *syscall,
                                      const zend_op **args, uint arg_count)
{
  char *dst = NULL;

  if (MATCH_ANY(syscall, "fwrite", "fputs", "fputc", "fputcsv")) {
    dst = get_resource_filename(get_arg_zval(execute_data, args[0]));
  } else if (MATCH_ANY(syscall, "file_put_contents", "touch", "mkdir", "rmdir", "unlink")) {
    dst = Z_STRVAL_NP(get_arg_zval(execute_data, args[0]));
  } else if (MATCH_ANY(syscall, "rename", "copy")) {
    dst = Z_STRVAL_NP(get_arg_zval(execute_data, args[1]));
  }

  if (dst != NULL) {
    uint squashed = squash_trailing_slash(dst);
    size_t request_id_str_len;
    char request_id_buffer[32];

    if (flock(fileno(current_app->evo_taint_log), LOCK_EX) != 0) {
      ERROR("Failed to acquire exclusive lock on the evo taint log file!\n");
      return;
    }

    request_id_str_len = snprintf(request_id_buffer, 32, "%d", current_request_id);
    fwrite(request_id_buffer, sizeof(char), request_id_str_len, current_app->evo_taint_log);
    fwrite("|", sizeof(char), 1, current_app->evo_taint_log);
    fwrite(dst, sizeof(char), strlen(dst), current_app->evo_taint_log);
    fwrite("\n", sizeof(char), 1, current_app->evo_taint_log);

    if (flock(fileno(current_app->evo_taint_log), LOCK_UN) != 0)
      ERROR("Failed to unlock the evo taint log file!\n");

    if (squashed > 0)
      dst[squashed] = '/';
  }
}

static uint evo_commit_expiring_taint_patches(evo_taint_t *taint)
{
  uint patch_count = 0;
  pending_cfg_patch_t *patch, *next;
  cfg_opcode_edge_t *opcode_edge;

  for (patch = taint->patch_list; patch != NULL; patch = next, patch_count++) {
    next = patch->next_pending;
    switch (patch->type) {
      case PENDING_ROUTINE_EDGE:
        plog(current_app, PLOG_TYPE_CFG, "add (commit) taint-verified: %04d 0x%x -> 0x%x\n",
             patch->from_index, patch->from_routine->routine_hash, patch->to_routine->routine_hash);

        if (!cfg_has_routine_edge(patch->app->cfg, patch->from_routine, patch->from_index,
                                  patch->to_routine, patch->to_index, patch->user_level)) {
          cfg_add_routine_edge(patch->app->cfg, patch->from_routine, patch->from_index,
                               patch->to_routine, patch->to_index, patch->user_level);
        }
        break;
      case PENDING_OPCODE_EDGE:
        plog(current_app, PLOG_TYPE_CFG, "add (commit) taint-verified: 0x%x %04d -> %04d\n",
             patch->routine->routine_hash, patch->from_index, patch->to_index);

        opcode_edge = routine_cfg_lookup_opcode_edge(patch->routine, patch->from_index,
                                                     patch->to_index);
        if (opcode_edge == NULL) {
          routine_cfg_add_opcode_edge(patch->routine, patch->from_index, patch->to_index,
                                      patch->user_level);
        } else {
          opcode_edge->user_level = patch->user_level;
        }
        break;
      case PENDING_NODE_USER_LEVEL:
        patch->cfg_opcode->user_level = patch->user_level; /* not enough info to log it */
        break;
    }
    PROCESS_FREE(patch);
  }

  return patch_count;
}

static void evo_expire_taint()
{
  evo_taint_t *tail;
  uint64 hash;
  uint patch_count;

  while (evo_taint_queue.tail != NULL) {
    tail = (evo_taint_t *) evo_taint_queue.tail;
    if (tail->request_id > current_request_id)
      break; /* enable a bogus scenario for testing purposes */
    if ((current_request_id - tail->request_id) < EVO_TAINT_EXPIRATION)
      break;

    patch_count = evo_commit_expiring_taint_patches(tail);

    if (current_app != NULL) {
      if (is_db_taint(tail)) {
        plog(current_app, PLOG_TYPE_DB_MOD,
             "Expiring taint %s.%s[%lld] with %d patches\n",
             tail->table_name, tail->column_name, tail->table_key, patch_count);
      } else {
        plog(current_app, PLOG_TYPE_DB_MOD,
             "Expiring taint %s with %d patches\n", tail->file_path, patch_count);
      }
    }

    hash = hash_evo_taint(tail);
    sctable_remove_value(&evo_taint_table, hash, tail);
    scqueue_dequeue(&evo_taint_queue);
    PROCESS_FREE((char *) tail->table_name); // ok for file taint?
    PROCESS_FREE((char *) tail->column_name);
    PROCESS_FREE(tail);
  }
}

static bool increment_file_synch_request_id()
{
  size_t size;
  char buffer[32];
  FILE *rid = current_app->request_id_file;

  if (flock(fileno(rid), LOCK_EX) != 0) {
    ERROR("Failed to acquire exclusive lock on the request id file!\n");
    return false;
  }

  fseek(rid, 0, SEEK_END);
  size = ftell(rid);

  if (size > 0) {
    rewind(rid);
    if (fread(buffer, sizeof(char), size, rid) < size) {
      ERROR("Failed to read the request id file!\n");
      return false;
    } else {
      current_request_id = atoi(buffer) + 1;
    }
  } else {
    current_request_id = 0;
  }

  rewind(rid);
  size = snprintf(buffer, 32, "%d", current_request_id);
  fwrite(buffer, sizeof(char), size + 1, rid);
  fflush(rid);

  if (flock(fileno(rid), LOCK_UN) != 0)
    ERROR("Failed to release exclusive lock on the request id file!\n");

  return true;
}

static void admin_site_modification(uint request_id, evo_taint_t *taint_id)
{
  uint64 hash = hash_evo_taint(taint_id);

  if (!sctable_has_value(&evo_taint_table, hash, taint_id)) {
    evo_taint_t *taint = PROCESS_NEW(evo_taint_t);

    memset(taint, 0, sizeof(evo_taint_t));
    if (is_db_taint(taint_id)) {
      taint->table_key = taint_id->table_key;
      taint->table_name = strdup(taint_id->table_name);  /* alloc process scope: until expiration */
      taint->column_name = strdup(taint_id->column_name);
    } else {
      taint->file_path = strdup(taint_id->file_path);
    }
    taint->request_id = request_id;
    taint->patch_list = NULL;
    sctable_add(&evo_taint_table, hash, taint);

    scqueue_enqueue(&evo_taint_queue, taint);
  } /* else evo: reset the request_id and remove/enqueue */

  if (current_app != NULL) {
    if (is_db_taint(taint_id)) {
      plog(current_app, PLOG_TYPE_DB_MOD,
           "%s.%s[%lld] with hash 0x%llx at request id %lld\n", taint_id->table_name,
           taint_id->column_name, taint_id->table_key, hash, current_request_id);
    } else {
      plog(current_app, PLOG_TYPE_FILE_MOD, "%s\n", taint_id->file_path);
    }
  }
}

static uint64 read_last_evo_start()
{
  size_t size;
  char buffer[32];
  FILE *evo_start = current_app->evo_start_file;
  uint64 last_evo_start;

  if (flock(fileno(evo_start), LOCK_SH) != 0) {
    ERROR("Failed to acquire shared lock on the evo start file!\n");
    return false;
  }

  fseek(evo_start, 0, SEEK_END);
  size = ftell(evo_start);

  if (size > 0) {
    rewind(evo_start);
    if (fread(buffer, sizeof(char), size, evo_start) < size) {
      ERROR("Failed to read the evo start file!\n");
      return 0;
    } else {
      last_evo_start = atoi(buffer);
    }
  } else {
    last_evo_start = 0;
  }

  if (flock(fileno(evo_start), LOCK_SH) != 0)
    ERROR("Failed to release exclusive lock on the evo start file!\n");

  return last_evo_start;
}

static void evo_db_state_synch()
{
  int status;
  uint64 last_evo_start = read_last_evo_start();

  evo_expire_taint();

  if ((current_request_id - evo_last_synch_request_id) > EVO_TAINT_EXPIRATION) {
    if (current_request_id < EVO_TAINT_EXPIRATION)
      evo_last_synch_request_id = 0;
    else
      evo_last_synch_request_id = (current_request_id - EVO_TAINT_EXPIRATION);
  }

  if (last_evo_start > 0 && last_evo_start >= evo_last_synch_request_id) {
    //plog(current_app, PLOG_TYPE_AD_HOC, "Evo DB state synch at request id %lld from last synch point %lld\n",
    //     current_request_id, evo_last_synch_request_id);

    status = mysql_stmt_execute(evo_state_query_stmt); /* evo_last_synch_request_id */
    if (status == 0) {
      status = mysql_stmt_bind_result(evo_state_query_stmt, bind_evo_state_result);
      if (status == 0) {
        while (true) {
          status = mysql_stmt_fetch(evo_state_query_stmt);
          if (status == 0) {
            if (evo_state_request_id > evo_last_synch_request_id) {
              evo_last_synch_request_id = evo_state_request_id + 1;
              evo_last_unchecked_request_id = evo_last_synch_request_id;
            }

            evo_update_taint_id.table_key = evo_state_table_key;
            admin_site_modification(evo_state_request_id, &evo_update_taint_id);
          } else {
            if (status == 1)
              ERROR("Failed to fetch results from the evo synch statement: %d\n", status);
            break;
          }
        }
      } else {
        ERROR("Failed to bind results from the evo synch statement: %d\n", status);
      }
    } else {
      ERROR("Failed to execute the evo synch statement: %d\n", status);
    }
  } else {
    //plog(current_app, PLOG_TYPE_AD_HOC, "No evo at request id %lld (last evo start at %lld, "
    //     "last synch from %lld)\n", current_request_id, last_evo_start, evo_last_synch_request_id);
  }

  request_has_taint = (evo_taint_queue.head != NULL);
  set_monitor_mode(request_has_taint ? MONITOR_MODE_ALL : MONITOR_MODE_CALLS);
  update_request_id();
}

static void notify_evo_start()
{
  size_t size;
  char buffer[32];
  FILE *evo_start = current_app->evo_start_file;

  if (flock(fileno(evo_start), LOCK_EX) != 0) {
    ERROR("Failed to acquire exclusive lock on the evo start file!\n");
    return;
  }

  fseek(evo_start, 0, SEEK_END);
  size = ftell(evo_start);

  rewind(evo_start);
  size = snprintf(buffer, 32, "%d", current_request_id);
  fwrite(buffer, sizeof(char), size + 1, evo_start);
  fflush(evo_start);

  if (flock(fileno(evo_start), LOCK_UN) != 0)
    ERROR("Failed to release exclusive lock on the evo start file!\n");
}

static void evo_db_state_notify()
{
  int status;

  if (current_session.user_level < 2 || !IS_CFI_DB()) {
    ERROR("Notifying evo state in a request that cannot initiate an expansion.\n");
    return;
  }

  status = mysql_stmt_execute(evo_start_query_stmt);
  if (status == 0) {
    status = mysql_stmt_bind_result(evo_start_query_stmt, bind_evo_start_result);
    if (status == 0) {
      status = mysql_stmt_fetch(evo_start_query_stmt);
      if (status == 0) {
        if (evo_start_count > 0)
          notify_evo_start();
        mysql_stmt_fetch(evo_start_query_stmt); /* N.B.: clear the query state */
      } else if (status == 1) {
        ERROR("Failed to fetch results from the evo start statement: %d\n", status);
      }
    } else {
      ERROR("Failed to bind results from the evo start statement: %d\n", status);
    }
  } else {
    ERROR("Failed to execute the evo start statement: %d\n", status);
  }
}

static void evo_file_state_synch()
{
  uint request_id, evo_taint_window_start;
  size_t line_mark = 0;
  ssize_t line_len;
  char *line = NULL, *sep, *file_path, request_id_buffer[16];

  if (flock(fileno(current_app->evo_taint_log), LOCK_SH) != 0) {
    ERROR("Failed to acquire shared lock the evo taint log file!\n");
    return;
  }

  if (current_request_id >= EVO_TAINT_EXPIRATION)
    evo_taint_window_start = (current_request_id - EVO_TAINT_EXPIRATION);
  else
    evo_taint_window_start = 0;

  evo_expire_taint();

  fseek(current_app->evo_taint_log, taint_log_synch_pos, SEEK_SET);
  while ((line_len = getline(&line, &line_mark, current_app->evo_taint_log)) != -1) {
    sep = strchr(line, '|');
    if (sep != NULL) {
      line[line_len-1] = '\0';
      memcpy(request_id_buffer, line, sep - line);
      request_id_buffer[sep - line] = '\0';
      request_id = atoi(request_id_buffer);

      if (request_id < evo_taint_window_start)
        continue;

      file_path = sep + 1; // now `sep` contains the file path

      plog(current_app, PLOG_TYPE_CFG, "Found taint on file %s at request #%d\n",
           file_path, request_id);

      {
        evo_taint_t taint_id = { 0, NULL, { file_path }, 0ULL, NULL };
        admin_site_modification(request_id, &taint_id);
      }
    }
  }

  free(line);
  taint_log_synch_pos = ftell(current_app->evo_taint_log);
  if (flock(fileno(current_app->evo_taint_log), LOCK_UN) != 0)
    ERROR("Failed to unlock the evo taint log file!\n");

  request_has_taint = (evo_taint_queue.head != NULL);
  set_monitor_mode(request_has_taint ? MONITOR_MODE_ALL : MONITOR_MODE_CALLS);
}

static void evo_commit_request_patches()
{
  pending_cfg_patch_t *patch;

  scarray_iterator_t *i = scarray_iterator_start(&pending_cfg_patches);
  while ((patch = (pending_cfg_patch_t *) scarray_iterator_next(i)) != NULL) {
    switch (patch->type) {
      case PENDING_ROUTINE_EDGE:
        write_routine_edge(patch->app, patch->from_routine->routine_hash, patch->from_index,
                           patch->to_routine->routine_hash, patch->to_index, patch->user_level);
        break;
      case PENDING_OPCODE_EDGE:
        write_op_edge(patch->app, patch->routine->routine_hash, patch->from_index,
                      patch->to_index, patch->user_level);
        break;
      default: ;
    }
    /* enqueue for final commit at taint expiration */
    patch->next_pending = patch->taint_source->patch_list;
    patch->taint_source->patch_list = patch;
  }
  scarray_iterator_end(i);
}

static uint evo_key_md5(const byte *key_value, uint key_len)
{
  byte md5_raw[MD5_DIGEST_LENGTH], *md5_walk = md5_raw;
  uint i, md5 = 0;

  MD5(key_value, key_len, md5_raw);

  for (i = 0; i < 4; i++, md5_walk++) { /* reverse endian */
    md5 <<= 8;
    md5 |= *md5_walk;
  }

  return md5;
}

static uint evo_key_int_md5(zend_ulong src)
{
  char string[0x20];

  snprintf(string, 0x20, "%d", src);
  return evo_key_md5((const byte *) string, strlen(string));
}

static bool is_key_index(int *indexes, uint index_count, int target)
{
  for (; index_count > 0; index_count--, indexes++) {
    if (target == *indexes)
      return true;
  }
  return false;
}

void db_fetch_trigger(uint32_t field_count, const char **table_names, const char **column_names,
                      const zval **values)
{
  if (IS_CFI_DB() && field_count > 0 && current_session.user_level < 2) {
    taint_variable_t *var;
    site_modification_t *mod;

    zend_execute_data *execute_data = EG(current_execute_data)->prev_execute_data;
    zend_op_array *op_array = &EX(func)->op_array;
    const zend_op *op = EX(opline);

    uint table_name_hash = hash_string(table_names[0]); /* assuming single-table updates */
    evo_key_t *evo_key = sctable_lookup(&evo_key_table, table_name_hash);
    int i, j, key_column_indexes[EVO_MAX_KEY_COLUMN_COUNT];
    zend_ulong key = 0;
    uint64 field_hash;

    memset(key_column_indexes, -1, sizeof(key_column_indexes));

    if (evo_key == NULL || field_count < (evo_key->column_count + 1))
      return; /* not a taintable table, or no key, or only key */

#ifdef DEBUG_PERMALINK
    for (i = 0; i < field_count; i++) {
      if (strcmp(table_names[0], "wp_options") == 0 && strcmp(column_names[i], "option_value") == 0 &&
          strcmp(Z_STRVAL_P(values[i]), "/%postname%/") == 0)
        is_post_permalink = true;
    }
#endif

    for (j = 0; j < evo_key->column_count; j++) {
      for (i = 0; i < field_count; i++) {
        if (strcmp(column_names[i], evo_key->column_names[j]) == 0) {
          if (evo_key->is_md5) {
            switch (Z_TYPE_P(values[i])) {
              case IS_LONG:
                key ^= evo_key_int_md5(Z_LVAL_P(values[i]));
                break;
              case IS_STRING: {
                const char *key_name = Z_STRVAL_P(values[i]);
                key ^= evo_key_md5((const byte *) key_name, strlen(key_name));
              } break;
              default:
                ERROR("Cannot read evo key of Z_TYPE %d\n", Z_TYPE_P(values[i]));
                return;
            }
          } else {
            switch (Z_TYPE_P(values[i])) {
              case IS_LONG:
                key = Z_LVAL_P(values[i]);
                break;
              case IS_STRING:
                ZEND_HANDLE_NUMERIC(Z_STR_P(values[i]), key);
                break;
              default:
                ERROR("Cannot read evo key of Z_TYPE %d\n", Z_TYPE_P(values[i]));
                return;
            }
          }
          key_column_indexes[j] = i;
          break;
        }
      }
      if (key_column_indexes[j] < 0)
        return; /* can't apply taint, b/c dunno if these values are modified by admin */
    }

    for (i = 0; i < field_count; i++) {
      if (!is_key_index(key_column_indexes, evo_key->column_count, i)) {
        evo_taint_t *found, lookup = { 0, table_names[0], { column_names[i] }, key, NULL };
        field_hash = hash_evo_taint(&lookup);

        // plog(current_app, PLOG_TYPE_AD_HOC, "Looking for evo %s.%s[%lld] with hash 0x%llx\n",
        //      table_names[0], column_names[i], key, field_hash);

        found = (evo_taint_t *) sctable_lookup_value(&evo_taint_table, field_hash, &lookup);
        if (found == NULL)
          continue;

        /* per-request alloc assumes request-static values in app space are not possible */
        mod = REQUEST_NEW(site_modification_t);
        mod->type = SITE_MOD_DB;
        mod->db_table = request_strdup(table_names[i]);
        mod->db_column = request_strdup(column_names[i]);
        mod->source = found;

        if (Z_TYPE_P(values[i]) == IS_STRING)
          mod->db_value = request_strdup(Z_STRVAL_P(values[i]));
        else
          mod->db_value = NULL;

        var = create_taint_variable(site_relative_path(current_app, op_array),
                                    op, TAINT_TYPE_SITE_MOD, mod);

        plog(current_app, PLOG_TYPE_TAINT, "db-fetch at %04d(L%04d)%s\n",
             op_context.cur.index, op->lineno, site_relative_path(current_app, op_array));

        taint_var_add(current_app, values[i], var);
      }
    }
  }
}

static inline bool is_db_write(const char *query) {
  const evo_query_t *command = db_write_commands[0], *last = db_write_commands[DB_WRITE_COMMAND_COUNT];

  for (; command < last; command++) {
    if (strncasecmp(query, command->query, command->len) == 0)
      return true;
  }
  return false;
}

/* returns true if admin is logged in and the query appears to be a DB write */
monitor_query_flags_t db_query(const char *query)
{
  monitor_query_flags_t flags = 0;
  bool is_admin = current_session.user_level >= 2, is_write = is_db_write(query);

  if (IS_CFI_DB() && is_admin) { // && TAINT_ALL) {
    if (is_write) {
      /* N.B.: op_context may be stale if the call is already verified */
      zend_execute_data *execute_data = EG(current_execute_data)->prev_execute_data; // db is builtin
      zend_op_array *op_array = &execute_data->func->op_array;
      uint index = execute_data->opline - op_array->opcodes;

      plog(current_app, PLOG_TYPE_DB, "query {%s} at %04d(L%04d)%s\n", query, index,
           execute_data->opline->lineno, site_relative_path(current_app, op_array));
    }
  }

  if (IS_CFI_DB()) {
    if (is_admin)
      flags |= MONITOR_QUERY_FLAG_IS_ADMIN;
  } /* else always skip the triggers */
  if (is_write)
    flags |= MONITOR_QUERY_FLAG_IS_WRITE;
  return flags;
}

static void propagate_internal_dataflow()
{
  zend_bool has_taint = false, is_transfer = false, free_src = false;
  zend_dataflow_t *dataflow_frame = dataflow_stack_base;

  if (!IS_CFI_EVO() || op_context.execute_data == NULL || !request_has_taint)
    return;

  for (; dataflow_frame < dataflow_hooks->dataflow_stack; dataflow_frame++) {
    if (dataflow_frame->src == NULL) {
      has_taint = true; // just mark the hashtable
    } else if (dataflow_frame->dst == NULL) {
      free_src = true; // just free taint from the src
    } else {
      if (dataflow_frame->container == (HashTable *) int2p(1ULL)) {
        dataflow_frame->container = NULL; // indicates a taint transfer within already-marked ht
        is_transfer = true;
      }

      if (is_return(op_context.cur.op->opcode)) {
        /* N.B.: must be in a builtin function: cannot ref cur_frame.execute_data! */
        if (op_context.implicit_taint != NULL && !is_transfer) {
          plog(current_app, PLOG_TYPE_TAINT, "implicit I%d->0x%llx at a builtin\n",
               op_context.implicit_taint->id, (uint64) dataflow_frame->dst);
          taint_var_add(current_app, dataflow_frame->dst, op_context.implicit_taint->taint);
          op_context.implicit_taint->last_applied_op_index = 0; /* not at any op */
          has_taint = true;
        } else {
          has_taint = propagate_zval_taint_quiet(current_app, true, dataflow_frame->src, "internal",
                                                 dataflow_frame->dst, "internal");
        }
      } else {
        zend_op_array *stack_frame = &op_context.execute_data->func->op_array;

        if (op_context.implicit_taint != NULL && !is_transfer &&
            op_context.implicit_taint->last_applied_op_index != op_context.cur.index) {
          plog(current_app, PLOG_TYPE_TAINT, "implicit I%d->0x%llx at %04d(L%04d)%s\n",
               op_context.implicit_taint->id, (uint64) dataflow_frame->dst, op_context.cur.index,
               op_context.cur.op->lineno, site_relative_path(current_app, stack_frame));

          taint_var_add(current_app, dataflow_frame->dst, op_context.implicit_taint->taint);
          op_context.implicit_taint->last_applied_op_index = op_context.cur.index;
          has_taint = true;
        } else {
          if (op_context.cfm != NULL && op_context.cfm->cfg != NULL &&
              op_context.cfm->cfg->routine_hash == 0x63be5693 &&
              Z_TYPE_P(dataflow_frame->src) == IS_STRING &&
              (strncmp(Z_STRVAL_P(dataflow_frame->src), "a:69:{s:47:", 11) == 0 ||
               strcmp(Z_STRVAL_P(dataflow_frame->src), "/%postname%/") == 0))
            SPOT("wait\n"); // in get_option() where writes to `alloptions`

          has_taint = propagate_zval_taint(current_app, op_context.execute_data, stack_frame,
                                           op_context.cur.op, true, dataflow_frame->src, "internal",
                                           dataflow_frame->dst, "internal");
        }
      }
    }

    if (is_transfer || free_src) {
      taint_var_remove(dataflow_frame->src);
      is_transfer = free_src = false;
    } else if (has_taint && dataflow_frame->container != NULL) {
      dataflow_frame->container->u.v.reserve |= HASH_RESERVE_TAINT;
    }
  }

  reset_dataflow_stack();
}
