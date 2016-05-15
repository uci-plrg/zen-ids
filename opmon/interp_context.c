#include "php.h"
#include <pthread.h>
#include <openssl/md5.h>

#include <mysql.h>

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
#include "compile_context.h"
#include "interp_context.h"

// # define OPMON_LOG_SELECT 1

#define MAX_STACK_FRAME_shadow_stack 0x1000
#define MAX_STACK_FRAME_exception_stack 0x100
#define MAX_STACK_FRAME_lambda_stack 0x100
#define MAX_STACK_FRAME_fcall_stack 0x20
#define ROUTINE_NAME_LENGTH 256
#define FLUSH_MASK 0xff

#define IS_TOP_FRAME(ex) \
  (VM_FRAME_KIND((ex)->frame_info) == VM_FRAME_TOP_CODE || \
   VM_FRAME_KIND((ex)->frame_info) == VM_FRAME_TOP_FUNCTION)

#define IS_SAME_FRAME(a, b) ((a).execute_data == (b).execute_data && (a).opcodes == (b).opcodes)

#define MAX_BIGINT_CHARS 32
#define EVO_STATE_QUERY "SELECT request_id, table_name, column_name, table_key " \
                        "FROM opmon_evolution "                                  \
                        "WHERE request_id > %d"
#define EVO_STATE_QUERY_MAX_LENGTH (sizeof(EVO_STATE_QUERY) + MAX_BIGINT_CHARS)
#define EVO_STATE_QUERY_FIELD_COUNT 4

#define EVO_TAINT_EXPIRATION 1000

/* N.B.: the evo triggers are using last_insert_id() as set by this query! */
#define EVO_NEXT_REQUEST_ID "UPDATE opmon_request_sequence " \
                            "SET request_id = last_insert_id(request_id + 1)"

#define EVO_QUERY(query) { query, sizeof(query) - 1 }

typedef struct _implicit_taint_t {
  uint id;
  const zend_op *end_op;
  taint_variable_t *taint;
  int last_applied_op_index;
} implicit_taint_t;

typedef struct _stack_frame_t {
  zend_execute_data *execute_data;
  zend_op *opcodes;
  zend_uchar opcode;
  union {
    uint op_index;
    uint throw_index;
  };
  control_flow_metadata_t cfm;
  const char *last_builtin_name;
  implicit_taint_t *implicit_taint;
} stack_frame_t;

typedef stack_frame_t exception_frame_t;

typedef struct _lambda_frame_t {
  const char *name;
} lambda_frame_t;

typedef enum _stack_state_t {
  STACK_STATE_NONE,
  STACK_STATE_UNWINDING,
  STACK_STATE_RETURNING,
  STACK_STATE_RETURNED,
  STACK_STATE_CALL
} stack_state_t;

typedef struct _stack_event_t {
  stack_state_t state;
  zend_uchar last_opcode;
  zend_execute_data *return_target;
  const char *return_target_name;
} stack_event_t;

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
  const char *table_name;
  const char *column_name;
  unsigned long long table_key;
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
};

typedef struct _evo_query_t {
  const char *query;
  size_t len;
} evo_query_t;

static MYSQL *db_connection = NULL;

static const evo_query_t evo_next_request_id = EVO_QUERY(EVO_NEXT_REQUEST_ID);

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

static lambda_frame_t lambda_stack[MAX_STACK_FRAME_lambda_stack];
static lambda_frame_t *lambda_frame;

static stack_frame_t void_frame;
static stack_frame_t prev_frame;
static stack_frame_t cur_frame;

static stack_event_t stack_event;

static user_session_t current_session;

static uint64 current_request_id = 0;

static uint op_execution_count = 0;

static pthread_t first_thread_id;

static zend_op entry_op;

static zend_execute_data *trace_start_frame = NULL;
static bool trace_all_opcodes = false;

static bool request_blocked = false;

typedef struct _taint_call_chain_t {
  zend_execute_data *start_frame;
  implicit_taint_t *taint_source;
} taint_call_chain_t;

static sctable_t implicit_taint_table; // todo: allocate entries per request and clear via erase()
static implicit_taint_t pending_implicit_taint = { 0, NULL, NULL, -1 };
static uint implicit_taint_id = 0;
static taint_call_chain_t implicit_taint_call_chain = { 0 };
static scarray_t pending_cfg_patches;

static sctable_t evo_key_table;
static sctable_t evo_taint_table;
static scqueue_t evo_taint_queue;

#define CONTEXT_ENTRY 0xffffffffU

static void evo_state_synch();
static void evo_commit_request_patches();

static uint get_first_executable_index(zend_op *opcodes)
{
  uint i;
  for (i = 0; zend_get_opcode_name(opcodes[i].opcode) == NULL; i++) ;
  return i;
}

static bool evo_taint_comparator(void *a, void *b)
{
  evo_taint_t *first = (evo_taint_t *) a;
  evo_taint_t *second = (evo_taint_t *) b;

  return (strcmp(first->table_name, second->table_name) == 0 &&
          strcmp(first->column_name, second->column_name) == 0 &&
          first->table_key == second->table_key);
}

void initialize_interp_context()
{
  uint i;

  memset(&void_frame, 0, sizeof(stack_frame_t));
  memset(&prev_frame, 0, sizeof(stack_frame_t));
  memset(&cur_frame, 0, sizeof(stack_frame_t));

  // fake node for entry point
  memset(&entry_op, 0, sizeof(zend_op));
  entry_op.opcode = ENTRY_POINT_OPCODE;
  entry_op.extended_value = ENTRY_POINT_EXTENDED_VALUE;
  void_frame.opcodes = &entry_op;
  void_frame.opcode = entry_op.opcode;
  void_frame.cfm.cfg = routine_cfg_new(ENTRY_POINT_HASH);
  cur_frame = prev_frame = void_frame;
  routine_cfg_assign_opcode(cur_frame.cfm.cfg, ENTRY_POINT_OPCODE,
                            ENTRY_POINT_EXTENDED_VALUE, 0, 0, USER_LEVEL_TOP);

  memset(exception_stack, 0, 2 * sizeof(exception_frame_t));
  exception_frame = exception_stack + 1;

  memset(lambda_stack, 0, 2 * sizeof(lambda_frame_t));
  lambda_frame = lambda_stack + 1;

  stack_event.state = STACK_STATE_NONE;
  stack_event.last_opcode = 0;

  first_thread_id = pthread_self();

  current_session.user_level = USER_LEVEL_BOTTOM;
  current_session.active = false;

  if (IS_CFI_EVO()) {
    implicit_taint_table.hash_bits = 7;
    sctable_init(&implicit_taint_table);

    evo_key_table.hash_bits = 6;
    sctable_init(&evo_key_table);

    for (i = 0; i < EVO_KEY_COUNT; i++)
      sctable_add(&evo_key_table, hash_string(evo_keys[i].table_name), &evo_keys[i]);

    evo_taint_table.hash_bits = 8;
    evo_taint_table.comparator = evo_taint_comparator;
    sctable_init(&evo_taint_table);
    SCQUEUE_INIT(&evo_taint_queue, evo_taint_t, prev, next);

    scarray_init(&pending_cfg_patches);

    db_connection = mysql_init(NULL);
    if (mysql_real_connect(db_connection, "localhost", "wordpressuser", "$<r!p+5A3e", "wordpress", 0, NULL, 0) == NULL)
      ERROR("Failed to connect to the database!\n");
    else
      SPOT("Connected to the database!\n");
  }
}

void initialize_interp_app_context(application_t *app)
{
  stack_frame_t *base_frame = PROCESS_NEW(stack_frame_t); // mem: why not static?
  memset(base_frame, 0, sizeof(stack_frame_t));
  base_frame->opcodes = &entry_op;
  base_frame->opcode = entry_op.opcode;
  base_frame->cfm.cfg = routine_cfg_new(ENTRY_POINT_HASH);
  base_frame->cfm.app = app;
  app->base_frame = (void *) base_frame;
  routine_cfg_assign_opcode(base_frame->cfm.cfg, ENTRY_POINT_OPCODE,
                            ENTRY_POINT_EXTENDED_VALUE, 0, 0, USER_LEVEL_TOP);

  base_frame->cfm.dataset = dataset_routine_lookup(app, ENTRY_POINT_HASH);
  if (base_frame->cfm.dataset == NULL)
    write_node(app, ENTRY_POINT_HASH, routine_cfg_get_opcode(base_frame->cfm.cfg, 0), 0);
}

void destroy_interp_app_context(application_t *app)
{
  routine_cfg_free(((stack_frame_t *) app->base_frame)->cfm.cfg);
  PROCESS_FREE(app->base_frame);

  if (IS_CFI_EVO())
    mysql_close(db_connection);
}

void interp_request_boundary(bool is_request_start)
{
  if (IS_CFI_EVO()) {
    if (is_request_start) {
      if (mysql_real_query(db_connection, evo_next_request_id.query, evo_next_request_id.len) != 0)
        ERROR("Failed to get the next request id: %s.\n", mysql_sqlstate(db_connection));
      else
        current_request_id = mysql_insert_id(db_connection);
      evo_state_synch();
    }

    if (!is_request_start) {
      sctable_clear(&implicit_taint_table);
      implicit_taint_id = 0;
      implicit_taint_call_chain.start_frame = NULL;

      if (!request_blocked)
        evo_commit_request_patches();
      pending_cfg_patches.size = 0; /* evo: need to free them if request blocked */
      request_blocked = false;
    }
  }
}

static void push_exception_frame()
{
  INCREMENT_STACK(exception_stack, exception_frame);
  *exception_frame = cur_frame;
}

static void
evaluate_routine_edge(stack_frame_t *from_frame, stack_frame_t *to_frame, uint to_index)
{
  control_flow_metadata_t *from_cfm = &from_frame->cfm;
  zend_op *from_op = &from_frame->opcodes[from_frame->op_index];
  bool verified = false, add = false;

  if (current_session.user_level >= 2)
    return;

  if (from_cfm->dataset != NULL) {
    if (dataset_verify_routine_edge(from_cfm->app, from_cfm->dataset, from_frame->op_index, to_index,
                                    to_frame->cfm.cfg->routine_hash, current_session.user_level)) {
      verified = true;
    }
  }
  if (!verified && cfg_has_routine_edge(from_cfm->app->cfg, from_cfm->cfg, from_frame->op_index,
                                        to_frame->cfm.cfg, to_index, current_session.user_level)) {
    verified = true;
  }

  if (!verified) {
    if (to_index == 0) {
      if (implicit_taint_call_chain.start_frame == NULL) {
        if (cur_frame.implicit_taint != NULL) {
          implicit_taint_call_chain.start_frame = from_frame->execute_data;
          implicit_taint_call_chain.taint_source = cur_frame.implicit_taint;
          add = true;

          plog(from_cfm->app, PLOG_TYPE_TAINT, "call chain activated at %04d(L%04d) %s -> %s\n",
               from_frame->op_index, from_op->lineno, from_cfm->routine_name, to_frame->cfm.routine_name);
        }
      } else {
        add = true;

        plog(from_cfm->app, PLOG_TYPE_TAINT, "call chain allows %04d(L%04d) %s -> %s\n",
             from_frame->op_index, from_op->lineno, from_cfm->routine_name, to_frame->cfm.routine_name);
      }
    } else {
      ERROR("Dataset evolution does not support Exception edges.\n");
    }
  }

  if (write_request_edge(add, from_cfm->app, from_cfm->cfg->routine_hash, from_frame->op_index,
                         to_frame->cfm.cfg->routine_hash, to_index, current_session.user_level)) {
    if (!verified) {
      if (add) {
        pending_cfg_patch_t *patch = PROCESS_NEW(pending_cfg_patch_t);
        patch->type = PENDING_ROUTINE_EDGE;
        patch->taint_source = GET_TAINT_VAR_EVO_SOURCE(implicit_taint_call_chain.taint_source->taint);
        patch->app = from_cfm->app;
        patch->from_routine = from_cfm->cfg;
        patch->to_routine = to_frame->cfm.cfg;
        patch->from_index = from_frame->op_index;
        patch->to_index = to_index;
        patch->user_level = current_session.user_level;
        patch->next_pending = NULL;
        scarray_append(&pending_cfg_patches, patch);

        plog(from_cfm->app, PLOG_TYPE_CFG, "add (pending) taint-verified: %04d(L%04d) %s -> %s\n",
             from_frame->op_index, from_op->lineno, from_cfm->routine_name, to_frame->cfm.routine_name);
      } else {
        request_blocked = true;
        if (to_index == 0) {
          plog(from_cfm->app, PLOG_TYPE_CFG, "call unverified: %04d(L%04d) %s -> %s\n",
               from_frame->op_index, from_op->lineno, from_cfm->routine_name, to_frame->cfm.routine_name);
        } else {
          plog(from_cfm->app, PLOG_TYPE_CFG, "throw unverified: %04d(L%04d) %s -> %s\n",
               from_frame->op_index, from_op->lineno, from_cfm->routine_name, to_frame->cfm.routine_name);
        }
        plog_stacktrace(from_cfm->app, PLOG_TYPE_CFG, to_frame->execute_data);
      }
    }
  }
}

static void generate_routine_edge(control_flow_metadata_t *from_cfm, uint from_index,
                                  routine_cfg_t *to_cfg, uint to_index)
{
  bool add_routine_edge = !cfg_has_routine_edge(from_cfm->app->cfg, from_cfm->cfg, from_index,
                                                to_cfg, to_index, current_session.user_level);

  if (add_routine_edge) {
    cfg_add_routine_edge(from_cfm->app->cfg, from_cfm->cfg, from_index, to_cfg, to_index,
                         current_session.user_level);
    write_routine_edge(from_cfm->app, from_cfm->cfg->routine_hash, from_index,
                       to_cfg->routine_hash, to_index, current_session.user_level);
  }

  write_request_edge(add_routine_edge, from_cfm->app, from_cfm->cfg->routine_hash, from_index,
                     to_cfg->routine_hash, to_index, current_session.user_level);
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

static void lookup_cfm_by_name(zend_execute_data *execute_data, zend_op_array *op_array,
                               control_flow_metadata_t *cfm)
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
    char *routine_name_buffer = malloc(strlen(routine_name)+10);
    sprintf(routine_name_buffer, "<missing>%s", routine_name);
    cfm->routine_name = (const char *)routine_name_buffer;
    cfm->cfg = NULL;
    cfm->dataset = NULL;
    ERROR("Failed to find opcodes for function %s\n", routine_name);
  } else {
    *cfm = *monitored_cfm;
  }
}

static void lookup_cfm(zend_execute_data *execute_data, zend_op_array *op_array,
                       control_flow_metadata_t *cfm)
{
  control_flow_metadata_t *monitored_cfm = get_cfm_by_opcodes_address(op_array->opcodes);
  if (monitored_cfm == NULL) {
    WARN("Failed to find opcodes for hash 0x%llx\n", hash_addr(op_array->opcodes));
    lookup_cfm_by_name(execute_data, op_array, cfm);
  } else {
    *cfm = *monitored_cfm;
  }
}

static inline zend_execute_data *find_return_target(zend_execute_data *execute_data)
{
  zend_execute_data *return_target = execute_data->prev_execute_data;
  while (return_target != NULL &&
         (return_target->func == NULL ||
          return_target->func->op_array.type == ZEND_INTERNAL_FUNCTION))
    return_target = return_target->prev_execute_data;
  return return_target;
}

static bool validate_return(zend_execute_data *execute_data)
{
  zend_execute_data *return_target = find_return_target(execute_data);
  return (stack_event.return_target == execute_data || stack_event.return_target == return_target);
}

static bool update_stack_frame(const zend_op *op) // true if the stack pointer changed
{
  zend_execute_data *execute_data = EG(current_execute_data), *prev_execute_data;
  zend_op_array *op_array = &execute_data->func->op_array;
  stack_frame_t new_cur_frame, new_prev_frame;

  if (op_array == NULL || op_array->opcodes == NULL)
    return false; // nothing to do

  // hack
  if (op_array->type == ZEND_EVAL_CODE) { // eval or lambda
    new_cur_frame.cfm = get_last_eval_cfm();
  } else {
    lookup_cfm(execute_data, op_array, &new_cur_frame.cfm);
  }

  //if (stack_event.state == STACK_STATE_UNWINDING)
  //  return false;

  if (execute_data == cur_frame.execute_data && op_array->opcodes == cur_frame.opcodes &&
      (execute_data->opline - op_array->opcodes) > get_first_executable_index(op_array->opcodes)) {
    prev_frame = cur_frame;
    cur_frame.op_index = (execute_data->opline - op_array->opcodes);
    cur_frame.opcode = op->opcode;
    switch (stack_event.state) {
      case STACK_STATE_CALL:
        ERROR("Stack frame did not change after STACK_STATE_CALL at opcode 0x%x. Still in %s\n",
              stack_event.last_opcode, cur_frame.cfm.routine_name);
        stack_event.state = STACK_STATE_NONE;
        break;
      case STACK_STATE_RETURNING:
        if (!validate_return(execute_data))
          ERROR("Failed to clear STACK_STATE_RETURNING. Now in %s\n", cur_frame.cfm.routine_name);
      case STACK_STATE_RETURNED:
        stack_event.state = STACK_STATE_NONE;
        break;
      default: ;
    }

    return false; // nothing to do
  }

  new_cur_frame.execute_data = execute_data;
  new_cur_frame.opcodes = op_array->opcodes;
  new_cur_frame.opcode = op->opcode;
  new_cur_frame.op_index = (execute_data->opline - op_array->opcodes);
  if (op_array->type == ZEND_EVAL_CODE) { // eval or lambda
    new_cur_frame.cfm = get_last_eval_cfm();
  } else {
    lookup_cfm(execute_data, op_array, &new_cur_frame.cfm);
  }

  if (IS_CFI_EVO()) {
    new_cur_frame.implicit_taint = (implicit_taint_t *) sctable_lookup(&implicit_taint_table,
                                                                       hash_addr(execute_data));
  }

  if (IS_SAME_FRAME(cur_frame, void_frame))
    cur_frame = *(stack_frame_t *) new_cur_frame.cfm.app->base_frame;

  switch (stack_event.state) {
    case STACK_STATE_CALL:
      stack_event.state = STACK_STATE_NONE;
      break;
    case STACK_STATE_RETURNING:
      if (validate_return(execute_data)) {
        stack_event.state = STACK_STATE_RETURNED;
      } else {
        ERROR("Expected return to %s but returned to %s\n", stack_event.return_target_name,
              new_cur_frame.cfm.routine_name);
      }
      break;
    default: ;
  }

  prev_execute_data = execute_data->prev_execute_data;
  while (prev_execute_data != NULL &&
         (prev_execute_data->func == NULL ||
          prev_execute_data->func->op_array.type == ZEND_INTERNAL_FUNCTION))
    prev_execute_data = prev_execute_data->prev_execute_data;
  if (prev_execute_data == NULL) {
    new_prev_frame = *(stack_frame_t *) new_cur_frame.cfm.app->base_frame;
  } else {
    zend_op_array *prev_op_array = &prev_execute_data->func->op_array;
    new_prev_frame.execute_data = prev_execute_data;
    new_prev_frame.opcodes = prev_op_array->opcodes;
    new_prev_frame.opcode = prev_execute_data->opline->opcode;
    new_prev_frame.op_index = (prev_execute_data->opline - prev_op_array->opcodes);
    new_prev_frame.implicit_taint = NULL;
    lookup_cfm(prev_execute_data, prev_op_array, &new_prev_frame.cfm);
  }

  if (stack_event.state == STACK_STATE_RETURNED && (execute_data->opline - op_array->opcodes) > 0) {
    PRINT("<0x%x> Routine return from %s to %s with opcodes at "PX"|"PX" and cfg "PX".\n",
          getpid(), cur_frame.cfm.routine_name, new_cur_frame.cfm.routine_name, p2int(execute_data),
          p2int(op_array->opcodes), p2int(cur_frame.cfm.cfg));
  } else if (new_cur_frame.cfm.cfg != NULL) {
    if (IS_CFI_EVO()) {
      evaluate_routine_edge(&new_prev_frame, &new_cur_frame, 0/*routine entry*/);
    } else {
      generate_routine_edge(&new_prev_frame.cfm, new_prev_frame.op_index,
                            new_cur_frame.cfm.cfg, 0/*routine entry*/);

      PRINT("<0x%x> Routine call from %s to %s with opcodes at "PX"|"PX" and cfg "PX"\n",
            getpid(), new_prev_frame.cfm.routine_name, new_cur_frame.cfm.routine_name,
            p2int(execute_data), p2int(op_array->opcodes), p2int(new_cur_frame.cfm.cfg));
    }
  }

  if (trace_start_frame == execute_data) {
    trace_start_frame = NULL;
    trace_all_opcodes = false;
  }

  cur_frame = new_cur_frame;
  prev_frame = new_prev_frame;
  return true;
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
}

static uint get_next_executable_index(uint from_index)
{
  zend_uchar opcode;
  uint i = from_index + 1;
  while (true) {
    opcode = cur_frame.opcodes[i].opcode;
    if (zend_get_opcode_name(opcode) != NULL)
      break;
    i++;
  }
  return i;
}

static uint get_previous_executable_index(uint from_index)
{
  if (from_index == 0) {
    ERROR("Can't get the index prior to zero!\n");
    return 0;
  } else {
    zend_uchar opcode;
    int i = from_index - 1;

    do {
      opcode = cur_frame.opcodes[i].opcode;
      if (zend_get_opcode_name(opcode) != NULL)
        break;
      i--;
    } while (i > 0);
    return i;
  }
}

static bool is_unconditional_fallthrough()
{
  uint from_index;
  if (prev_frame.execute_data == NULL)
    return true;

  if (cur_frame.op_index == 0)
    return true;

  switch (cur_frame.opcodes[cur_frame.op_index].opcode) {
    case ZEND_RECV:
    case ZEND_RECV_INIT:
    case ZEND_RECV_VARIADIC:
      return true;
  }

  switch (prev_frame.opcodes[prev_frame.op_index].opcode) {
    case ZEND_RETURN:
    case ZEND_JMP:
    case ZEND_JMPZ:
    case ZEND_JMPZNZ:
    case ZEND_JMPZ_EX:
    case ZEND_JMPNZ_EX:
    case ZEND_FE_RESET:
    case ZEND_BRK:
    case ZEND_CONT:
      return false;
  }
  from_index = (IS_SAME_FRAME(cur_frame, prev_frame) ? prev_frame.op_index  : 0);
  return cur_frame.op_index == get_next_executable_index(from_index);
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

#ifdef TAINT_IO
static request_input_type_t get_request_input_type(const zend_op *op)
{
  switch (op->opcode) {
    case ZEND_FETCH_R:  /* fetch a superglobal */
    case ZEND_FETCH_W:
    case ZEND_FETCH_RW:
    case ZEND_FETCH_IS:
    case ZEND_FETCH_FUNC_ARG:
      if (op->op1_type == IS_CONST && op->op2_type == IS_UNUSED) {
        const char *superglobal_name = Z_STRVAL_P(op->op1.zv);
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
      break;
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
}

#ifdef TAINT_IO
static void plog_system_output_taint(const char *category, zend_execute_data *execute_data,
                                     zend_op_array *stack_frame, const zend_op *op,
                                     const zend_op **args, uint arg_count)
{
  uint i;
  taint_variable_t *taint;

  plog_call(cur_frame.cfm.app, category, cur_frame.last_builtin_name, stack_frame, op, arg_count, args);
  for (i = 0; i < arg_count; i++) {
    taint = taint_var_get_arg(execute_data, args[i]);
    if (taint != NULL) {
      plog(cur_frame.cfm.app, PLOG_TYPE_TAINT, "%s in %s(#%d): %04d(L%04d)%s\n",
           category, cur_frame.last_builtin_name, i,
           OP_INDEX(stack_frame, op), op->lineno, site_relative_path(cur_frame.cfm.app, stack_frame));
    }
  }
}
#endif

static void post_propagate_builtin(zend_op_array *op_array, const zend_op *op)
{
  zend_execute_data *execute_data = cur_frame.execute_data;
  const zend_op *args[0x20];
  uint arg_count;

  inflate_call(execute_data, op_array, op, args, &arg_count);

  propagate_args_to_result(cur_frame.cfm.app, execute_data, op, args, arg_count,
                           cur_frame.last_builtin_name);

#ifdef TAINT_IO
  if (TAINT_ALL) {
    if (is_file_sink_function(cur_frame.last_builtin_name))
      plog_system_output_taint("<file-output>", execute_data, op_array, op, args, arg_count);
    else if (is_file_source_function(cur_frame.last_builtin_name))
      plog_call(cur_frame.cfm.app, "<file-input>", cur_frame.last_builtin_name, op_array, op, arg_count, args);
    else if (is_db_sink_function("mysqli_", cur_frame.last_builtin_name))
      plog_system_output_taint("<db-output>", execute_data, op_array, op, args, arg_count);
    else if (is_db_source_function("mysqli_", cur_frame.last_builtin_name))
      plog_call(cur_frame.cfm.app, "<db-input>", cur_frame.last_builtin_name, op_array, op, arg_count, args);
  }
#endif

  cur_frame.last_builtin_name = NULL;
}

static void fcall_executing(zend_execute_data *execute_data, zend_op_array *op_array, const zend_op *op)
{
  const zend_op *args[0x20];
  uint arg_count;
  const char *callee_name = EX(call)->func->common.function_name->val;

  /*
  if (strcmp(callee_name, "get_option") == 0) {
    trace_start_frame = execute_data;
    trace_all_opcodes = true;
    plog(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "Calling get_option at %04d(L%04d)%s\n",
         cur_frame.op_index, op->lineno, site_relative_path(cur_frame.cfm.app, op_array));
  } else if (strcmp(callee_name, "set_permalink_structure") == 0) {
    plog(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "Calling set_permalink_structure at %04d(L%04d)%s\n",
         cur_frame.op_index, op->lineno, site_relative_path(cur_frame.cfm.app, op_array));
  }

  if (strcmp(callee_name, "wp_load_alloptions") == 0) {
    trace_start_frame = execute_data;
    trace_all_opcodes = true;
    plog(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "Calling wp_load_alloptions at %04d(L%04d)%s\n",
         cur_frame.op_index, op->lineno, site_relative_path(cur_frame.cfm.app, op_array));
  }
  */

  inflate_call(execute_data, op_array, op, args, &arg_count);

  if (EX(call)->func->type == ZEND_INTERNAL_FUNCTION) {
    cur_frame.last_builtin_name = callee_name; // use after free!
  } else if (IS_CFI_EVO()) {

    stack_event.state = STACK_STATE_CALL;
    stack_event.last_opcode = op->opcode;

    taint_prepare_call(cur_frame.cfm.app, execute_data, args, arg_count);
  }

  if (trace_all_opcodes) {
    uint i;
    const zval *arg_value;

    plog(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "call %s(", callee_name);
    for (i = 0; i < arg_count; i++) {
      arg_value = get_arg_zval(execute_data, args[i]);
      switch (Z_TYPE_P(arg_value)) {
        case IS_UNDEF:
          plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "?");
          break;
        case IS_NULL:
          plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "null");
          break;
        case IS_TRUE:
          plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "true");
          break;
        case IS_FALSE:
          plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "false");
          break;
        case IS_STRING:
          plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "%.20s", Z_STRVAL_P(arg_value));
          break;
        case IS_LONG:
          plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "%d", Z_LVAL_P(arg_value));
          break;
        case IS_DOUBLE:
          plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "%f", Z_DVAL_P(arg_value));
          break;
        case IS_ARRAY:
          plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "<arr>");
          break;
        case IS_OBJECT:
          plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "<obj>");
          break;
        case IS_RESOURCE:
          plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "<res>");
          break;
        case IS_REFERENCE:
          plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "<ref>");
          break;
        case IS_CONSTANT:
          plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "<const>");
          break;
        case IS_INDIRECT:
          plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "<ind>");
          break;
        default:
          plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "<%d>", Z_TYPE_P(arg_value));
          break;
      }
      if (i < (arg_count - 1))
        plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, ", ");
    }
    plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, ")\n");

    plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "\t     %s(", callee_name);
    for (i = 0; i < arg_count; i++) {
      arg_value = get_arg_zval(execute_data, args[i]);
      plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "0x%llx", (uint64) arg_value);
      if (i < (arg_count - 1))
        plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, ", ");
    }
    plog_append(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, ")\n");
  }
}

static zend_op *find_spanning_block_tail(const zend_op *cur_op, zend_op_array *op_array)
{
  zend_op *top = op_array->opcodes, *walk = &op_array->opcodes[prev_frame.op_index - 1];

  while (walk > top) {
    switch (walk->opcode) {
      case ZEND_JMPZ:
      case ZEND_JMPZNZ:
      case ZEND_JMPZ_EX:
      case ZEND_JMPNZ_EX:
        if (walk->op2.jmp_addr > cur_op)
          return walk->op2.jmp_addr;
    }
    walk--;
  }
  return &op_array->opcodes[op_array->last - 1];
}

static void remove_implicit_taint()
{
  if (cur_frame.implicit_taint != NULL) {
    sctable_remove(&implicit_taint_table, hash_addr(cur_frame.execute_data));
    // REQUEST_FREE(cur_frame.implicit_taint);
    cur_frame.implicit_taint = NULL;
  }
}

void opcode_executing(const zend_op *op)
{
  zend_execute_data *execute_data = EG(current_execute_data);
  zend_op_array *op_array = &execute_data->func->op_array;
  const zend_op *last_executed_op, *cur_frame_previous_op = NULL, *jump_target = NULL;
  const zval *jump_predicate = NULL;
  bool stack_pointer_moved, is_loopback, caught_exception = false;
  bool opcode_verified = false, opcode_edge_needs_update = false;
  taint_variable_t *taint_lowers_op_user_level = NULL;
  uint op_user_level = USER_LEVEL_TOP;
  cfg_opcode_t *expected_opcode = NULL;

  update_user_session();

  op_execution_count++;
  if ((op_execution_count & FLUSH_MASK) == 0)
    flush_all_outputs(cur_frame.cfm.app);

  if (pthread_self() != first_thread_id) {
    ERROR("Multiple threads are not supported (started on 0x%x, current is 0x%x)\n",
          (uint) first_thread_id, (uint) pthread_self());
    exit(1);
  }

  if (op->opcode == ZEND_HANDLE_EXCEPTION) {
    SPOT("@ Processing ZEND_HANDLE_EXCEPTION in stack state %u of "PX"|"PX"\n",
          stack_event.state, p2int(execute_data), p2int(op_array->opcodes));
    if (stack_event.state == STACK_STATE_NONE) {
      //zend_op *throw_op = &cur_frame.opcodes[cur_frame.last_index];
      stack_event.state = STACK_STATE_UNWINDING;
      push_exception_frame();
      SPOT("Exception thrown at op %d in opcodes "PX"|"PX" of routine 0x%x\n",
           cur_frame.op_index, p2int(execute_data), p2int(op_array->opcodes),
           cur_frame.cfm.cfg->routine_hash);
    }
    return;
  }

  stack_pointer_moved = update_stack_frame(op);

  if (trace_all_opcodes) {
    plog(cur_frame.cfm.app, PLOG_TYPE_AD_HOC, "%04d(L%04d)%s:%s\n", OP_INDEX(op_array, op),
         op->lineno, site_relative_path(cur_frame.cfm.app, op_array), cur_frame.cfm.routine_name);
  } else {
    PRINT("\t@ %04d(L%04d)%s:%s\n", OP_INDEX(op_array, op), op->lineno,
          site_relative_path(cur_frame.cfm.app, op_array), cur_frame.cfm.routine_name);
  }

  is_loopback = (IS_SAME_FRAME(cur_frame, prev_frame) && prev_frame.op_index > cur_frame.op_index);
  if (cur_frame.op_index > 0)
    cur_frame_previous_op = &cur_frame.opcodes[get_previous_executable_index(cur_frame.op_index)];
  if (IS_SAME_FRAME(prev_frame, cur_frame) && !stack_pointer_moved) {
    last_executed_op = &prev_frame.opcodes[prev_frame.op_index];
  } else {
    last_executed_op = cur_frame_previous_op; /* assuming a call continuation */

    if (last_executed_op == NULL && cur_frame.op_index > 0)
      ERROR("Failed to identify the last executed op within a stack frame!\n");
  }

  if (IS_CFI_EVO()) {
    if (stack_pointer_moved && last_executed_op != NULL && last_executed_op->opcode == ZEND_DO_FCALL) {
      taint_propagate_return(cur_frame.cfm.app, execute_data, op_array, last_executed_op);

      if (execute_data == implicit_taint_call_chain.start_frame)
        implicit_taint_call_chain.start_frame = NULL;
    }

    if (!is_loopback && (cur_frame_previous_op == NULL || IS_FIRST_AFTER_ARGS(op)))
      taint_propagate_into_arg_receivers(cur_frame.cfm.app, execute_data, op_array, (zend_op *) op);

    if (last_executed_op != NULL) {
      if (!is_loopback && cur_frame.last_builtin_name != NULL &&
          last_executed_op->opcode == ZEND_DO_FCALL) {
        post_propagate_builtin(op_array,  last_executed_op);
      } else {
        PRINT("T %04d(L%04d)%s:%s\n\t", OP_INDEX(op_array, last_executed_op), last_executed_op->lineno,
              site_relative_path(cur_frame.cfm.app, op_array), cur_frame.cfm.routine_name);

        propagate_taint(cur_frame.cfm.app, execute_data, op_array, last_executed_op);
      }
    }
  }

  if (!current_session.active) {
    PRINT("<session> Inactive session while executing %s. User level is %d.\n",
          cur_frame.cfm.routine_name, current_session.user_level);
  }

#ifdef TAINT_IO
  if (IS_CFI_EVO()) {
    request_input_type_t input_type = get_request_input_type(op); // TODO: combine in following switch
    if (input_type != REQUEST_INPUT_TYPE_NONE && TAINT_ALL) {
      const zval *value = get_zval(execute_data, &op->result, op->result_type);
      if (value != NULL) {
        request_input_t *input = REQUEST_NEW(request_input_t);
        taint_variable_t *taint_var;

        input->type = input_type;
        input->value = NULL;

        taint_var = create_taint_variable(site_relative_path(cur_frame.cfm.app, op_array),
                                          op, TAINT_TYPE_REQUEST_INPUT, input);
        plog(cur_frame.cfm.app, PLOG_TYPE_TAINT, "create request input at %04d(L%04d)%s\n",
             OP_INDEX(op_array, op), op->lineno, site_relative_path(cur_frame.cfm.app, op_array));
        taint_var_add(cur_frame.cfm.app, value, taint_var);
      }
    }
  }
#endif

  switch (op->opcode) {
    case ZEND_DO_FCALL:
      fcall_executing(execute_data, op_array, (zend_op *) op);
      //if (cur_frame.implicit_taint != NULL && implicit_taint_call_chain.start_frame == NULL)
      //  implicit_taint_call_chain.start_frame = execute_data;
      break;
    case ZEND_INCLUDE_OR_EVAL:
      if (op->extended_value == ZEND_INCLUDE || op->extended_value == ZEND_REQUIRE) {
        stack_event.state = STACK_STATE_CALL;
        stack_event.last_opcode = op->opcode;
      }
      break;
    case ZEND_RETURN:
    case ZEND_RETURN_BY_REF: {
      control_flow_metadata_t return_target_cfm;
      zend_execute_data *return_target = find_return_target(execute_data);
      if (return_target == NULL) {
        stack_event.return_target = NULL;
        stack_event.return_target_name = "<base-frame>";
      } else {
        lookup_cfm(return_target, &return_target->func->op_array, &return_target_cfm);
        stack_event.return_target = return_target;
        stack_event.return_target_name = return_target_cfm.routine_name;
      }
      PRINT("Preparing return at op %d of %s to %s\n", cur_frame.op_index, cur_frame.cfm.routine_name,
            stack_event.return_target_name);
      stack_event.state = STACK_STATE_RETURNING;
    } break;
    case ZEND_FAST_RET:
      //exception_frame->suspended = false;
      stack_event.state = STACK_STATE_UNWINDING;
      break;
    case ZEND_CATCH:
      break;
    default:
      if (stack_event.state == STACK_STATE_UNWINDING)
        stack_event.state = STACK_STATE_NONE;
      break;
  }

  if (op->opcode == ZEND_CATCH) {
    if (stack_event.state == STACK_STATE_UNWINDING) {
      WARN("Exception at op %d of 0x%x caught at op index %d in opcodes "PX"|"PX" of 0x%x\n",
           exception_frame->throw_index,
           exception_frame->cfm.cfg->routine_hash,
           cur_frame.op_index,
           p2int(cur_frame.execute_data), p2int(cur_frame.opcodes),
           cur_frame.cfm.cfg->routine_hash);
      stack_event.state = STACK_STATE_NONE;

      if (exception_frame->execute_data == cur_frame.execute_data) {
        if (IS_CFI_EVO()) {
          ERROR("Dataset evolution does not support Exception edges.\n");
        } else {
          if (!routine_cfg_has_opcode_edge(cur_frame.cfm.cfg, exception_frame->throw_index,
                                           cur_frame.op_index)) {
            generate_opcode_edge(&cur_frame.cfm, exception_frame->throw_index,
                                 cur_frame.op_index);
          } else {
            PRINT("(skipping existing exception edge)\n");
          }
        }
      } else {
        if (IS_CFI_EVO()) {
          evaluate_routine_edge(exception_frame, &cur_frame, cur_frame.op_index);
        } else {
          generate_routine_edge(&exception_frame->cfm, exception_frame->throw_index,
                                cur_frame.cfm.cfg, cur_frame.op_index);
        }
      }
      DECREMENT_STACK(exception_stack, exception_frame);
      caught_exception = true;
      // what to do with implicit taint here?
    } // else it was matching the Exception type (and missed)
  } else if (stack_event.state == STACK_STATE_UNWINDING) {
    WARN("Executing op %s while unwinding an exception!\n", zend_get_opcode_name(op->opcode));
  }

  if (cur_frame.cfm.cfg == NULL) {
    ERROR("No cfg for opcodes at "PX"|"PX"\n", p2int(execute_data),
          p2int(execute_data->func->op_array.opcodes));
  } else {
    if (cur_frame.op_index >= cur_frame.cfm.cfg->opcodes.size) {
      ERROR("attempt to execute foobar op %u in opcodes "PX"|"PX" of routine 0x%x\n",
            cur_frame.op_index, p2int(execute_data), p2int(op_array->opcodes),
            cur_frame.cfm.cfg->routine_hash);
      return;
    }

    PRINT("@ Executing %s at index %u of 0x%x (user level %d)\n",
          zend_get_opcode_name(cur_frame.opcode), cur_frame.op_index,
          cur_frame.cfm.cfg->routine_hash, op_user_level);

    expected_opcode = routine_cfg_get_opcode(cur_frame.cfm.cfg, cur_frame.op_index);
    if (cur_frame.opcode != expected_opcode->opcode &&
        !is_alias(cur_frame.opcode, expected_opcode->opcode)) {
      ERROR("Expected opcode %s at index %u, but found opcode %s in opcodes "
            PX"|"PX" of routine 0x%x\n",
            zend_get_opcode_name(expected_opcode->opcode), cur_frame.op_index,
            zend_get_opcode_name(cur_frame.opcode), p2int(execute_data),
            p2int(op_array->opcodes), cur_frame.cfm.cfg->routine_hash);
      op_user_level = USER_LEVEL_TOP;
    } else {
      if (cur_frame.cfm.dataset != NULL)
        op_user_level = dataset_routine_get_node_user_level(cur_frame.cfm.dataset, cur_frame.op_index);
      if (expected_opcode->user_level < op_user_level)
        op_user_level = expected_opcode->user_level; // don't verify the op on this basis until the corresponding taint expires!
    }

    // slightly weak for returns: not checking continuation pc
    if (stack_event.state == STACK_STATE_RETURNED || stack_pointer_moved || is_loopback/*safe w/o goto*/ ||
        is_unconditional_fallthrough() || current_session.user_level >= op_user_level) {
      if (is_unconditional_fallthrough()) {
        PRINT("@ Verified fall-through %u -> %u in 0x%x\n",
              prev_frame.op_index, cur_frame.op_index,
              cur_frame.cfm.cfg->routine_hash);
      } else {
        PRINT("@ Verified node %u in 0x%x\n", cur_frame.op_index, cur_frame.cfm.cfg->routine_hash);
      }
      opcode_verified = true;
    } else if (!caught_exception) {
      uint i;
      bool edge_found = false, edge_changed = false;

      for (i = 0; i < cur_frame.cfm.cfg->opcode_edges.size; i++) {
        cfg_opcode_edge_t *edge = routine_cfg_get_opcode_edge(cur_frame.cfm.cfg, i);
        if (edge->from_index == prev_frame.op_index) { // TODO: check user level
          if (edge->to_index == cur_frame.op_index) {
            if (current_session.user_level < edge->user_level) {
              edge->user_level = current_session.user_level;
              edge_changed = true;
            }
            edge_found = true;
            break;
          }
        }
      }
      opcode_edge_needs_update = !edge_found || edge_changed;
      if (!opcode_edge_needs_update) {
        PRINT("@ Verified opcode edge %u -> %u\n", prev_frame.op_index, cur_frame.op_index);
        /* not marking `opcode_verified` because taint is required on every pass */
      }
    }
  }

  if (IS_CFI_EVO()) {
    if (pending_implicit_taint.end_op != NULL) {
      if (!opcode_verified) {
        implicit_taint_t *implicit = REQUEST_NEW(implicit_taint_t);

        *implicit = pending_implicit_taint;
        if (op == pending_implicit_taint.end_op)
          implicit->end_op = find_spanning_block_tail(op, op_array);
        implicit->id = implicit_taint_id++;
        sctable_add(&implicit_taint_table, hash_addr(execute_data), implicit);
        implicit->last_applied_op_index = -1;
        cur_frame.implicit_taint = implicit;

        plog(cur_frame.cfm.app, PLOG_TYPE_TAINT, "activating I%d from %04d(L%04d)-%04d(L%04d)%s\n",
             cur_frame.implicit_taint->id,
             OP_INDEX(op_array, op), op->lineno, OP_INDEX(op_array, implicit->end_op),
             implicit->end_op->lineno, site_relative_path(cur_frame.cfm.app, op_array));

        taint_lowers_op_user_level = pending_implicit_taint.taint;
      }

      pending_implicit_taint.end_op = NULL;
    } else if (cur_frame.implicit_taint != NULL) {
      if (op < cur_frame.implicit_taint->end_op) {
        const zval *lValue;

        switch (op->opcode) {
          case ZEND_ASSIGN_REF:
            lValue = get_zval(execute_data, &op->result, op->result_type);
            plog(cur_frame.cfm.app, PLOG_TYPE_TAINT, "implicit %s (I%d)->(0x%llx) at %04d(L%04d)%s\n",
                 zend_get_opcode_name(op->opcode), cur_frame.implicit_taint->id, (uint64) lValue,
                 OP_INDEX(op_array, op), op->lineno, site_relative_path(cur_frame.cfm.app, op_array));
            taint_var_add(cur_frame.cfm.app, lValue, cur_frame.implicit_taint->taint);
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
          /*
          also returns?
          */
            lValue = get_zval(execute_data, &op->op1, op->op1_type);
            plog(cur_frame.cfm.app, PLOG_TYPE_TAINT, "implicit %s (I%d)->(0x%llx) at %04d(L%04d)%s\n",
                 zend_get_opcode_name(op->opcode), cur_frame.implicit_taint->id, (uint64) lValue,
                 OP_INDEX(op_array, op), op->lineno, site_relative_path(cur_frame.cfm.app, op_array));
            taint_var_add(cur_frame.cfm.app, lValue, cur_frame.implicit_taint->taint);
            cur_frame.implicit_taint->last_applied_op_index = cur_frame.op_index;
        }

        /*
        plog(cur_frame.cfm.app, PLOG_TYPE_TAINT, "+implicit on %04d(L%04d)%s until %04d(L%04d)\n",
             OP_INDEX(op_array, op), op->lineno, site_relative_path(cur_frame.cfm.app, op_array),
             OP_INDEX(op_array, cur_frame.implicit_taint->end_op), cur_frame.implicit_taint->end_op->lineno);
        plog_taint_var(cur_frame.cfm.app, cur_frame.implicit_taint->taint, 0);
        */
        taint_lowers_op_user_level = cur_frame.implicit_taint->taint;

        if (is_return(op->opcode))
          remove_implicit_taint();
      } else {
        if (IS_SAME_FRAME(prev_frame, cur_frame) && !stack_pointer_moved &&
            last_executed_op->opcode == ZEND_JMP &&
            last_executed_op < cur_frame.implicit_taint->end_op) {
          taint_lowers_op_user_level = cur_frame.implicit_taint->taint; /* extend to bottom of branch diamond */
        }
        remove_implicit_taint();
      }
    }

    if (cur_frame.implicit_taint == NULL) {
      const zend_op *jump_op = op;

      while (true) {
        switch (jump_op->opcode) {
          case ZEND_JMPZ:
          case ZEND_JMPZNZ:
            if (jump_op->op2.jmp_addr > jump_op) {
              jump_target = jump_op->op2.jmp_addr;
              if (jump_predicate == NULL)
                jump_predicate = get_zval(execute_data, &jump_op->op1, jump_op->op1_type);
            }
            break;
          case ZEND_JMPZ_EX:
          case ZEND_JMPNZ_EX:
            if (jump_op->op2.jmp_addr > jump_op) {
              jump_op = jump_op->op2.jmp_addr;
              if (jump_predicate == NULL)
                jump_predicate = get_zval(execute_data, &jump_op->op1, jump_op->op1_type);
              continue;
              //jump_target = op->op2.jmp_addr;
              //jump_predicate = get_zval(execute_data, &op->result, op->result_type);
            }
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

#ifdef PLOG_CFG
    if (!opcode_verified && current_session.user_level < 2) {
      plog(cur_frame.cfm.app, PLOG_TYPE_CFG, "opcode unverified %s %04d(L%04d)%s\n",
           zend_get_opcode_name(op->opcode), OP_INDEX(op_array, op), op->lineno,
           site_relative_path(cur_frame.cfm.app, op_array));
    }
#endif
  }

  if (IS_CFI_TRAINING()) {
    if (opcode_edge_needs_update) {
      routine_cfg_add_opcode_edge(cur_frame.cfm.cfg, prev_frame.op_index, cur_frame.op_index,
                                  current_session.user_level);
      write_op_edge(cur_frame.cfm.app, cur_frame.cfm.cfg->routine_hash, prev_frame.op_index,
                    cur_frame.op_index, current_session.user_level);
    }
  } else {
    if (opcode_edge_needs_update && taint_lowers_op_user_level != NULL) {
      compiled_edge_target_t compiled_target;
      zend_op *from_op = &prev_frame.opcodes[prev_frame.op_index];

      pending_cfg_patch_t *patch = PROCESS_NEW(pending_cfg_patch_t);
      patch->type = PENDING_OPCODE_EDGE;
      patch->taint_source = GET_TAINT_VAR_EVO_SOURCE(taint_lowers_op_user_level);
      patch->app = cur_frame.cfm.app;
      patch->routine = cur_frame.cfm.cfg;
      patch->from_index = prev_frame.op_index;
      patch->to_index = cur_frame.op_index;
      patch->user_level = current_session.user_level;
      patch->next_pending = NULL;
      scarray_append(&pending_cfg_patches, patch);

      if (false) { // todo: also allow conditional branches to have op edges
        compiled_target = get_compiled_edge_target(from_op, prev_frame.op_index);
        if (compiled_target.type != COMPILED_EDGE_INDIRECT) {
          ERROR("Generating indirect edge from compiled target type %d (opcode 0x%x)\n",
               compiled_target.type, op->opcode);
        }
      }
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
}

static void evo_commit_expiring_taint_patches(evo_taint_t *taint)
{
  pending_cfg_patch_t *patch, *next;

  for (patch = taint->patch_list; patch != NULL; patch = next) {
    next = patch->next_pending;
    switch (patch->type) {
      case PENDING_ROUTINE_EDGE:
        cfg_add_routine_edge(patch->app->cfg, patch->from_routine, patch->from_index,
                             patch->to_routine, patch->to_index, patch->user_level);
        break;
      case PENDING_OPCODE_EDGE:
        routine_cfg_add_opcode_edge(patch->routine, patch->from_index, patch->to_index,
                                    patch->user_level);
        break;
      case PENDING_NODE_USER_LEVEL:
        patch->cfg_opcode->user_level = patch->user_level;
        break;
    }
    PROCESS_FREE(patch);
  }
}

static void evo_expire_taint()
{
  evo_taint_t *tail;
  uint64 hash;

  while (evo_taint_queue.tail != NULL) {
    tail = (evo_taint_t *) evo_taint_queue.tail;
    if (tail->request_id > current_request_id)
      break; /* enable a bogus scenario for testing purposes */
    if ((current_request_id - tail->request_id) < EVO_TAINT_EXPIRATION)
      break;

    evo_commit_expiring_taint_patches(tail);

    hash = hash_string(tail->table_name) ^ hash_string(tail->column_name) ^ tail->table_key; /* evo: shared code! */
    sctable_remove_value(&evo_taint_table, hash, tail);
    scqueue_dequeue(&evo_taint_queue);
    PROCESS_FREE((char *) tail->table_name);
    PROCESS_FREE((char *) tail->column_name);
    PROCESS_FREE(tail);
  }
}

static void db_site_modification(uint request_id, const char *table_name,
                                 const char *column_name, uint64 table_key)
{
  evo_taint_t lookup = { 0, table_name, column_name, table_key, NULL };
  uint64 hash = hash_string(table_name) ^ hash_string(column_name) ^ table_key; // crowd low bits

  if (!sctable_has_value(&evo_taint_table, hash, &lookup)) {
    evo_taint_t *taint = PROCESS_NEW(evo_taint_t);
    taint->request_id = request_id;
    taint->table_key = table_key;
    taint->table_name = strdup(table_name);
    taint->column_name = strdup(column_name);
    taint->patch_list = NULL;
    sctable_add(&evo_taint_table, hash, taint);

    scqueue_enqueue(&evo_taint_queue, taint);
  } /* else evo: reset the request_id and remove/enqueue */

  if (cur_frame.cfm.app != NULL)
    plog(cur_frame.cfm.app, PLOG_TYPE_DB_MOD, "%s.%s[%lld]\n", table_name, column_name, table_key);
}

static void evo_state_synch()
{
  char evo_state_query[EVO_STATE_QUERY_MAX_LENGTH];
  MYSQL_RES *result;
  MYSQL_ROW row;
  uint request_id;

  evo_expire_taint();

  if ((current_request_id - evo_last_synch_request_id) > EVO_TAINT_EXPIRATION) {
    if (current_request_id < EVO_TAINT_EXPIRATION)
      evo_last_synch_request_id = 0;
    else
      evo_last_synch_request_id = (current_request_id - EVO_TAINT_EXPIRATION);
  }

  snprintf(evo_state_query, EVO_STATE_QUERY_MAX_LENGTH, EVO_STATE_QUERY, evo_last_synch_request_id);
  if (mysql_real_query(db_connection, evo_state_query, strlen(evo_state_query)) != 0) {
    if (mysql_field_count(db_connection))
      ERROR("Failed to query the evolution state: %s.\n", mysql_sqlstate(db_connection));
  } else {
    result = mysql_store_result(db_connection);
    if (result == NULL) {
      if (mysql_field_count(db_connection))
        ERROR("Failed to store the evolution state query result: %s.\n", mysql_sqlstate(db_connection));
    } else {
      while ((row = mysql_fetch_row(result)) != NULL) {
        if (row[0] == NULL || strlen(row[0]) == 0)
          break;
        request_id = strtoul(row[0], NULL, 10);
        if (request_id > evo_last_synch_request_id)
          evo_last_synch_request_id = request_id;
        db_site_modification(request_id, row[1], row[2], strtoull(row[3], NULL, 10));
      }
      mysql_free_result(result);
    }
  }
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

void db_fetch(uint32_t field_count, const char **table_names, const char **column_names,
              const zval **values)
{
  if (IS_CFI_EVO() && field_count > 0 && current_session.user_level < 2) {
    taint_variable_t *var;
    site_modification_t *mod;

    zend_op *op = &cur_frame.opcodes[cur_frame.op_index];
    zend_op_array *op_array = &cur_frame.execute_data->func->op_array;

    uint table_name_hash = hash_string(table_names[0]); /* assuming single-table updates */
    evo_key_t *evo_key = sctable_lookup(&evo_key_table, table_name_hash);
    int i, j, key_column_indexes[EVO_MAX_KEY_COLUMN_COUNT];
    zend_ulong key = 0;
    uint64 field_hash;

    memset(key_column_indexes, -1, sizeof(key_column_indexes));

    if (evo_key == NULL || field_count < (evo_key->column_count + 1))
      return; /* not a taintable table, or no key, or only key */

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
        evo_taint_t *found, lookup = { 0, table_names[0], column_names[i], key, NULL };
        field_hash = table_name_hash ^ hash_string(column_names[i]) ^ key;

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

        var = create_taint_variable(site_relative_path(cur_frame.cfm.app, op_array),
                                    op, TAINT_TYPE_SITE_MOD, mod);

        plog(cur_frame.cfm.app, PLOG_TYPE_TAINT, "db-fetch at %04d(L%04d)%s\n",
             cur_frame.op_index, op->lineno, site_relative_path(cur_frame.cfm.app, op_array));
        taint_var_add(cur_frame.cfm.app, values[i], var);
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
  bool is_admin = current_session.user_level > 2;

  if (IS_CFI_EVO() && is_admin) { // && TAINT_ALL) {
    zend_op *op = &cur_frame.opcodes[cur_frame.op_index];
    zend_op_array *op_array = &cur_frame.execute_data->func->op_array;

    plog(cur_frame.cfm.app, PLOG_TYPE_DB, "query {%s} at %04d(L%04d)%s\n", query,
         cur_frame.op_index, op->lineno, site_relative_path(cur_frame.cfm.app, op_array));
  }

  if (IS_CFI_EVO()) {
    if (is_admin)
      flags |= MONITOR_QUERY_FLAG_IS_ADMIN;
  } /* else always skip the triggers */
  if (is_db_write(query))
    flags |= MONITOR_QUERY_FLAG_IS_WRITE;
  return flags;
}

zend_bool internal_dataflow(const zval *src, const char *src_name,
                            const zval *dst, const char *dst_name,
                            zend_bool is_internal_transfer)
{
  zend_bool has_taint = false;

  if (!IS_CFI_EVO() || cur_frame.execute_data == NULL)
    return has_taint;

  if (cur_frame.implicit_taint != NULL && !is_internal_transfer &&
      cur_frame.implicit_taint->last_applied_op_index != cur_frame.op_index) {
    zend_op_array *stack_frame = &cur_frame.execute_data->func->op_array;
    zend_op *op = &cur_frame.opcodes[cur_frame.op_index];

    plog(cur_frame.cfm.app, PLOG_TYPE_TAINT, "implicit %s(I%d)->%s(0x%llx) at %04d(L%04d)%s\n",
         src_name, cur_frame.implicit_taint->id, dst_name, (uint64) dst, cur_frame.op_index,
         op->lineno, site_relative_path(cur_frame.cfm.app, stack_frame));
    taint_var_add(cur_frame.cfm.app, dst, cur_frame.implicit_taint->taint);
    cur_frame.implicit_taint->last_applied_op_index = cur_frame.op_index;
    has_taint = true;
  } else {
    has_taint = propagate_zval_taint(cur_frame.cfm.app, cur_frame.execute_data,
                                     &cur_frame.execute_data->func->op_array,
                                     &cur_frame.opcodes[cur_frame.op_index], true,
                                     src, src_name, dst, dst_name);
  }

  if (has_taint && is_internal_transfer)
    taint_var_remove(src);

  return has_taint;
}

user_level_t get_current_user_level()
{
  return current_session.user_level;
}
