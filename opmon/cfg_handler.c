#include "php.h"
#include "SAPI.h"
#include "httpd.h"
#include "standard/url.h"
#include "util_script.h"
#include "util_filter.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "lib/script_cfi_utils.h"
#include "interp_context.h"
#include "event_handler.h"
#include "dataset.h"
#include "dataflow.h"
#include "taint.h"
#include "operand_resolver.h"
#include "cfg_handler.h"

// #define URL_DECODE 1
#define USER_LEVEL_SHIFT 26

static char session_file_path[CONFIG_FILENAME_LENGTH] = {0}, cfg_file_path[CONFIG_FILENAME_LENGTH] = {0};

static bool is_standalone_app = false;
static cfg_files_t standalone_cfg_files;
static void *standalone_dataset;

typedef struct _session_t {
  char id[256];
  uint hash;
} session_t;

#define HASH_ROUTINE_EDGE(from, to) ((((uint64)from) << 0x20) | ((uint64)to))

typedef struct _request_edge_t {
  uint from_index;
  uint from_routine_hash;
  uint to_index;
  uint to_routine_hash;
  uint user_level;
  struct _request_edge_t *next;
} request_edge_t;

typedef struct _request_state_t {
  bool is_in_request;
  bool is_new_request;
  uint64 request_id;
  application_t *app;
  request_rec *r;
  sctable_t *edges; // of `request_edge_t`
  uint pool_index;
  scarray_t *edge_pool;
} request_state_t;

static request_state_t request_state;
static const uint request_header_tag = REQUEST_HEADER_TAG;
static session_t session = { {0}, 0 };

static inline void fnull(size_t size, FILE *file)
{
  static const char NULL_BUFFER[256] = {0};
  fwrite(NULL_BUFFER, 1, size, file);
}

static inline void fopcode(cfg_opcode_t *opcode, FILE *file)
{
  fwrite(&opcode->opcode, sizeof(zend_uchar), 1, file);
  if (opcode->opcode == ZEND_INCLUDE_OR_EVAL)
    fwrite(&opcode->extended_value, sizeof(zend_uchar), 1, file);
  else
    fnull(1, file);
  fwrite(&opcode->line_number, sizeof(ushort), 1, file);
}

static uint get_timestamp(void)
{
  uint lo, hi;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));

  return (hi << 0xc) | (lo >> 0x14);
}

static bool is_media_file(char *buffer)
{
  if (strncmp("%PDF", buffer, strlen("%PDF")) == 0)
    return true;

  return false;
}

static void write_request_entry(cfg_files_t *cfg_files)
{
  SPOT("Starting request %08lld 0x%lx: %s\n", request_state.request_id,
       request_state.r->request_time, request_state.r->the_request);

  fprintf(cfg_files->request, "<request-time> 0x%lx\n", request_state.r->request_time);
  fprintf(cfg_files->request, "<request> %s\n", request_state.r->the_request);
  fprintf(cfg_files->request, "<requested-file> %s\n", request_state.r->filename);
  fprintf(cfg_files->request, "<protocol> %s\n", request_state.r->protocol);
  fprintf(cfg_files->request, "<method> %s\n", request_state.r->method);
  fprintf(cfg_files->request, "<content-type> %s\n", request_state.r->content_type);
  fprintf(cfg_files->request, "<content-encodings> %s\n", request_state.r->content_encoding);
  fprintf(cfg_files->request, "<client-ip> %s\n", request_state.r->useragent_ip);
  fprintf(cfg_files->request, "<status> %s\n", request_state.r->status_line);
  fprintf(cfg_files->request, "<arguments> %s\n", request_state.r->args);

  {
    uint i;
    apr_table_entry_t *entry;
    const apr_array_header_t *entries = NULL;
    entries = apr_table_elts(request_state.r->headers_in);
    entry = (apr_table_entry_t *) entries->elts;
    for (i = 0; i < entries->nelts; i++) {
      if (strcasecmp(entry[i].key, "Cookie") == 0) {
        const char *set = entry[i].val, *mark;
        while (true) {
          mark = strchr(set, ';');
          if (strncmp(set, "ucinetid_auth=", 14) == 0) { // hide ucinet auth token!
            set += 78;
            if (set[0] == ';')
              set += 2;
            continue;
          }
          if (mark == NULL) {
            fprintf(cfg_files->request, "<cookie> %s\n", set);
            break;
          } else {
            fprintf(cfg_files->request, "<cookie> ");
            fwrite(set, sizeof(char), mark - set, cfg_files->request);
            fprintf(cfg_files->request, "\n");
            set = mark+2;
          }
        }
      } else {
        fprintf(cfg_files->request, "<header> %s=%s\n", entry[i].key, entry[i].val);
      }
    }
  }

  switch (request_state.r->method_number) {
    case M_GET: {
      uint i;
      apr_table_entry_t *entry;
      apr_table_t *table;
      apr_array_header_t *entries;
      ap_args_to_table(request_state.r, &table);
      entries = (apr_array_header_t *) table;
      entry = (apr_table_entry_t *) entries->elts;
      for (i = 0; i < entries->nelts; i++)
        fprintf(cfg_files->request, "<get-variable> %s=%s\n", entry[i].key, entry[i].val);
    } break;
    case M_POST: {
      char buffer[SAPI_POST_BLOCK_SIZE], *set, *mark;
      apr_size_t size, tail;
#ifdef URL_DECODE
      char decode_buffer[SAPI_POST_BLOCK_SIZE];
      apr_size_t decode_size, decode_fragment_size = 0;

      decode_buffer[0] = '\0';
#endif

      fprintf(cfg_files->request, "<post-variable> ");

      if (SG(request_info).request_body == NULL) {
        fprintf(cfg_files->request, "(null)");
      } else {
        php_stream_rewind(SG(request_info).request_body);
        while (true) {
          size = SG(request_info).request_body->ops->read(SG(request_info).request_body, buffer,
                                                          SAPI_POST_BLOCK_SIZE);
          if (size == 0)
            break;
          if (is_media_file(buffer)) { /* ignore all subsequent parameters--impossible to find the delimiter */
            fprintf(cfg_files->request, "[media]");
            break;
          }
          set = buffer;
          do { /* write up to the next post parameter (delimited by '&') */
            mark = memchr(set, '&', size - (set - buffer));
            if (mark == NULL) // || mark > buffer + size)
              break;
#ifdef URL_DECODE
            decode_size = mark - set;
            decode_fragment_size = 0;
            if (size == SAPI_POST_BLOCK_SIZE) {
              if (set[decode_size-1] == '%')
                decode_fragment_size = 1;
              else if (set[decode_size-2] == '%')
                decode_fragment_size = 2;
              decode_size = decode_size - decode_fragment_size;
            }
            strncat(decode_buffer, set, decode_size);
            set += decode_size;
            decode_size = php_url_decode(decode_buffer, decode_size);
            fwrite(decode_buffer, sizeof(char), decode_size, cfg_files->request);
            fprintf(cfg_files->request, "\n<post-variable> ");
            if (decode_fragment_size > 0)
              strncpy(decode_buffer, set, decode_fragment_size);
            else
              decode_buffer[0] = '\0';
#else
            fwrite(set, sizeof(char), mark - set, cfg_files->request);
            fprintf(cfg_files->request, "\n<post-variable> ");
#endif
            set = mark + 1;
          } while (set < (buffer + SAPI_POST_BLOCK_SIZE));
#ifdef URL_DECODE
          strncat(decode_buffer, set, size - (set - buffer));
          decode_size = php_url_decode(decode_buffer, size - (set - buffer));
          fwrite(decode_buffer, sizeof(char), decode_size, cfg_files->request);
#else
          tail = 0;
          if (size > 0)
            tail = size - (set - buffer);
          if (tail > 0)
            fwrite(set, sizeof(char), tail, cfg_files->request);
#endif
        }
      }
      fprintf(cfg_files->request, "\n");
    } break;
    default:
      ERROR("Unknown request type %s\n", request_state.r->method);
  }

  fprintf(cfg_files->request, "<request-id> |%08lld\n", request_state.request_id);
}

void init_cfg_handler()
{
  memset(&request_state, 0, sizeof(request_state_t));
}

void destroy_cfg_handler()
{
  if (request_state.edges != NULL) {
    request_edge_t *edge;
    scarray_iterator_t *i = scarray_iterator_start(request_state.edge_pool);
    while ((edge = (request_edge_t *) scarray_iterator_next(i)) != NULL)
      PROCESS_FREE(edge);
    scarray_iterator_end(i);
    scarray_destroy(request_state.edge_pool);
    PROCESS_FREE(request_state.edge_pool);
    request_state.edge_pool = NULL;
    sctable_destroy(request_state.edges);
    PROCESS_FREE(request_state.edges);
    request_state.edges = NULL;
  }
}

bool is_standalone_mode()
{
  return is_standalone_app;
}

static void open_output_files_in_dir(cfg_files_t *cfg_files, char *cfg_file_path, const char *mode)
{
  char *cfg_file_truncate = cfg_file_path + strlen(cfg_file_path);

  SPOT("Attempting to open output files in dir %s\n", cfg_file_path);

#define OPEN_CFG_FILE(filename, file_field) \
  do { \
    *cfg_file_truncate = '\0'; \
    strcat(cfg_file_path, (filename)); \
    cfg_files->file_field = fopen(cfg_file_path, mode); \
    if (cfg_files->file_field == NULL) \
      ERROR("Failed to open cfg file cfg_files."#file_field"\n"); \
    else \
      SPOT("Successfully opened cfg_files."#file_field"\n"); \
  } while (0);

  OPEN_CFG_FILE("node.run", node);
  OPEN_CFG_FILE("op-edge.run", op_edge);
  OPEN_CFG_FILE("routine-edge.run", routine_edge);
  OPEN_CFG_FILE("routine-catalog.tab", routine_catalog);
  OPEN_CFG_FILE("persistence.log", persistence);
  if (IS_OPCODE_DUMP_ENABLED())
    OPEN_CFG_FILE("opcodes.log", opcode_log);
  if (!is_standalone_app) {
    OPEN_CFG_FILE("request.tab", request);
    if (IS_REQUEST_EDGE_OUTPUT_ENABLED())
      OPEN_CFG_FILE("request-edge.run", request_edge);
  }
#undef OPEN_CFG_FILE
}

static void open_output_files(const char *script_path)
{
  char standalone_cfg_file_path[CONFIG_FILENAME_LENGTH] = {0}, run_id[24] = {0};
  struct stat dirinfo;
  time_t timestamp;
  struct tm *calendar;

  time(&timestamp);
  calendar = localtime(&timestamp);

  setup_base_path(standalone_cfg_file_path, "runs", script_path);

  if (stat(standalone_cfg_file_path, &dirinfo) != 0) {
    if (mkdir(standalone_cfg_file_path, 0777) != 0) {
      ERROR("Failed to create the cfg file directory %s\n", standalone_cfg_file_path);
      return;
    }
  }

  sprintf(run_id, "/%d.%d-%d.%d/",
          calendar->tm_yday, calendar->tm_hour, calendar->tm_min, getpid());
  strcat(standalone_cfg_file_path, run_id);

  if (mkdir(standalone_cfg_file_path, 0777) != 0) {
    ERROR("Failed to create the cfg file directory %s\n", standalone_cfg_file_path);
    return;
  }

  open_output_files_in_dir(&standalone_cfg_files, standalone_cfg_file_path, "w");
}

void starting_script(const char *script_path)
{
  struct stat dirinfo;
  char *resolved_path = zend_resolve_path(script_path, strlen(script_path));

  is_standalone_app = true;

  if (is_static_analysis()) {
    const char *analysis = get_static_analysis();
    //char cfg_file_path[256] = {0};

    SPOT("Starting static analysis '%s' of file %s on pid %d\n",
         analysis, resolved_path, getpid());

    setup_base_path(cfg_file_path, "runs", analysis);
    strcat(cfg_file_path, "/");

    if (stat(cfg_file_path, &dirinfo) != 0) {
      if (mkdir(cfg_file_path, 0777) != 0) {
        ERROR("Failed to create the cfg file directory %s\n", cfg_file_path);
        efree(resolved_path);
        return;
      }
    }

    open_output_files_in_dir(&standalone_cfg_files, cfg_file_path, "a");
    standalone_dataset = load_dataset(analysis);
  } else {
    SPOT("starting_script %s on pid %d\n", resolved_path, getpid());

    open_output_files(resolved_path);
    standalone_dataset = load_dataset(resolved_path);
  }

  init_operand_resolver();

  efree(resolved_path);
}

void server_startup()
{
  char session_id[32] = {0};
  char *cfg_file_truncate;
  struct stat dirinfo;
  time_t timestamp;
  struct tm *calendar;
  FILE *session_catalog_file;

  setup_base_path(session_file_path, "runs", "webserver");

  if (stat(session_file_path, &dirinfo) != 0) {
    SPOT("Attempting to create webserver session directory %s\n", session_file_path);
    if (mkdir(session_file_path, 0755) != 0) {
      PERROR("Failed to create the webserver session directory: %s\n", session_file_path);
      return;
    }
    SPOT("Successfully created webserver session directory %s\n", session_file_path);
  }

  time(&timestamp);
  calendar = localtime(&timestamp);

  strcat(session_file_path, "/");
  cfg_file_truncate = session_file_path + strlen(session_file_path);

  strcat(session_file_path, "session_catalog.tab");

  session_catalog_file = fopen(session_file_path, "a");
  if (session_catalog_file == NULL) {
    PERROR("Failed to open the session catalog file %s for writing\n", session_file_path);
    return;
  }
  SPOT("Successfully opened the session catalog file %s for appending\n", session_file_path);

  sprintf(session_id, "session.%d.%d-%d.%d",
          calendar->tm_yday, calendar->tm_hour, calendar->tm_min, getpid());

  fprintf(session_catalog_file, "%s\n", session_id);
  fflush(session_catalog_file);
  fclose(session_catalog_file);
  SPOT("Successfully appended to the session catalog file %s\n", session_file_path);

  *cfg_file_truncate = '\0';
  strcat(session_file_path, session_id);

  if (mkdir(session_file_path, 0755) != 0) {
    PERROR("Failed to create the webserver session directory %s\n", session_file_path);
    return;
  }
  chmod(session_file_path, 0777); // running as root here, but child runs as www-data

  SPOT("Successfully created webserver session directory %s\n", session_file_path);
}

void worker_startup()
{
  char session_id[24] = {0};
  struct stat dirinfo;

  strcpy(cfg_file_path, session_file_path);
  if (stat(cfg_file_path, &dirinfo) != 0) {
    ERROR("Cannot find the webserver cfg directory %s\n", cfg_file_path);
    return;
  }
  SPOT("Child %d found the CFG file path %s\n", getpid(), cfg_file_path);
  fflush(stderr);

  strcat(cfg_file_path, "/");
  sprintf(session_id, "worker-%u/", getpid());
  strcat(cfg_file_path, session_id);

  SPOT("Child %d looking for worker directory %s\n", getpid(), cfg_file_path);
  fflush(stderr);

  if (stat(cfg_file_path, &dirinfo) != 0) {
    if (mkdir(cfg_file_path, 0755) != 0) {
      PERROR("Failed to create worker directory %s\n", cfg_file_path);
      return;
    }
    SPOT("Successfully created worker directory %s\n", cfg_file_path);
  }

  init_operand_resolver();
}

void cfg_initialize_application(application_t *app)
{
  if (is_standalone_app) {
    app->cfg_files = &standalone_cfg_files;
    app->dataset = standalone_dataset;
  } else {
    char app_cfg_file_path[256] = {0};
    struct stat dirinfo;

    strcpy(app_cfg_file_path, cfg_file_path);
    strcat(app_cfg_file_path, app->name);
    strcat(app_cfg_file_path, "/");

    if (stat(app_cfg_file_path, &dirinfo) != 0) {
      if (mkdir(app_cfg_file_path, 0777) != 0) {
        ERROR("Failed to create the app cfg file directory %s\n", app_cfg_file_path);
        return;
      }
    }

    if (app->cfg_files != NULL)
      ERROR("Overwriting app cfg files!\n");

    app->cfg_files = PROCESS_NEW(cfg_files_t);
    open_output_files_in_dir((cfg_files_t *) app->cfg_files, app_cfg_file_path, "w");

    app->dataset = load_dataset(app->name);
  }

  initialize_interp_app_context(app);
}

void cfg_destroy_application(application_t *app)
{
  cfg_files_t *cfg_files = (cfg_files_t *) app->cfg_files;

  PRINT("  === Close cfg files\n");

  if (cfg_files != NULL)  {
    fclose(cfg_files->node);
    fclose(cfg_files->op_edge);
    fclose(cfg_files->routine_edge);
    fclose(cfg_files->routine_catalog);
    fclose(cfg_files->persistence);
    if (!is_standalone_app) {
      fclose(cfg_files->request);
      if (IS_REQUEST_EDGE_OUTPUT_ENABLED())
        fclose(cfg_files->request_edge);
    }
    if (!is_standalone_app)
      PROCESS_FREE(cfg_files);
    app->cfg_files = NULL;
  }

  destroy_interp_app_context(app);
}

void cfg_request_boundary(bool is_request_start, uint64 request_id)
{
  request_state.is_in_request = is_request_start;

  if (request_state.is_in_request) {
    cfg_files_t *cfg_files;
    php_server_context_t *context = (php_server_context_t *) SG(server_context);
    request_state.r = context->r;
    request_state.is_new_request = true;
    request_state.request_id = request_id;

    request_state.app = locate_application(request_state.r->filename);
    cfg_files = (cfg_files_t *) request_state.app->cfg_files;

    fprintf(cfg_files->persistence, "@ %08lld 0x%lx: %s\n", request_state.request_id,
            request_state.r->request_time, request_state.r->the_request);

    write_request_entry(cfg_files);

    if (request_state.edges == NULL) { // lazy construct b/c standalone mode doesn't need it
      request_state.edge_pool = PROCESS_NEW(scarray_t);
      scarray_init(request_state.edge_pool);
      request_state.edges = PROCESS_NEW(sctable_t);
      request_state.edges->hash_bits = 10;
      sctable_init(request_state.edges);
    } else { // clear the request edges
      sctable_clear(request_state.edges);
      request_state.pool_index = 0;
    }

    if (PS(id) != NULL && strcmp(PS(id)->val, session.id) != 0) {
      strcpy(session.id, PS(id)->val);
      session.hash = hash_string(session.id);
      SPOT("New session id %s (0x%x) on pid 0x%x\n", session.id, session.hash, getpid());
      /* Patched OPMON_REPLAY_AUTH in WordPress to accept any user_logged_in_* token
      if (strcmp(session.id, "9491nc08len3diid9t7r5757e5") == 0 / * ||
          strcmp(session.id, "i6ivdkcgdmurtjcffudk5frna4") == 0 * /) {
        SPOT("Faking user level 10 for truncated session\n");
        set_opmon_user_level(10);
      }
      */
    }
  } else {
    if (request_state.app != NULL) {
      flush_all_outputs(request_state.app);
      request_state.app = NULL;
    }
  }
}

/* 3 x 4 bytes: { routine_hash | opcode | index } */
void write_node(application_t *app, uint routine_hash, cfg_opcode_t *opcode, uint index)
{
  cfg_files_t *cfg_files = (cfg_files_t *) app->cfg_files;

  PRINT("Write node 0x%x #%d 0x%01x to cfg\n", routine_hash, index, opcode->opcode);
  fwrite(&routine_hash, sizeof(uint), 1, cfg_files->node);
  fopcode(opcode, cfg_files->node);
  fwrite(&index, sizeof(uint), 1, cfg_files->node);
}

/* 3 x 4 bytes: { routine_hash | user_level (6) from_index (26) | to_index } */
void write_op_edge(application_t *app, uint routine_hash, uint from_index, uint to_index,
                   user_level_t user_level)
{
  cfg_files_t *cfg_files = (cfg_files_t *) app->cfg_files;

  PRINT("Write op-edge 0x%x #%d -> #%d to cfg\n", routine_hash, from_index, to_index);

  from_index |= (user_level << USER_LEVEL_SHIFT);

  fwrite(&routine_hash, sizeof(uint), 1, cfg_files->op_edge);
  fwrite(&from_index, sizeof(uint), 1, cfg_files->op_edge);
  fwrite(&to_index, sizeof(uint), 1, cfg_files->op_edge);
}

/* 4 x 4 bytes: { from_routine_hash | user_level (6) from_index (26) |
 *                to_routine_hash | to_index }
 */
bool write_request_edge(bool is_new_in_process, application_t *app, uint from_routine_hash,
                        uint from_index, uint to_routine_hash, uint to_index,
                        user_level_t user_level)
{
  cfg_files_t *cfg_files = (cfg_files_t *) app->cfg_files;
  request_edge_t *edge_entry = NULL, *new_edge = NULL;
  uint64 key = HASH_ROUTINE_EDGE(from_routine_hash, to_routine_hash);
  uint packed_from_index;

  if (!is_new_in_process && !is_standalone_app) {
    request_edge_t *next = (request_edge_t *) sctable_lookup(request_state.edges, key);
    edge_entry = next;
    while (next != NULL) {
      if (from_routine_hash == next->from_routine_hash && from_index == next->from_index &&
          to_routine_hash == next->to_routine_hash && to_index == next->to_index) {
        new_edge = next;
        if (new_edge->user_level <= user_level)
          return false; // nothing new about this edge
        new_edge->user_level = user_level;
        break;
      }
      next = next->next;
    }
  }

  if (IS_REQUEST_EDGE_OUTPUT_ENABLED()) {
    PRINT("Write routine-edge {0x%x #%d} -> {0x%x #%d} to cfg\n",
          from_routine_hash, from_index, to_routine_hash, to_index);

    packed_from_index = from_index | (user_level << USER_LEVEL_SHIFT);

    if (request_state.is_new_request) {
      uint timestamp = get_timestamp();

      fwrite(&request_header_tag, sizeof(uint), 1, cfg_files->request_edge);
      fwrite(&request_state.request_id, sizeof(uint), 1, cfg_files->request_edge);
      fwrite(&session.hash, sizeof(uint), 1, cfg_files->request_edge);
      fwrite(&timestamp, sizeof(uint), 1, cfg_files->request_edge);

      /* Make an entry point if the stack policy didn't see it */
      if (from_routine_hash != BASE_FRAME_HASH) {
        uint entry_point_hash = BASE_FRAME_HASH;
        uint entry_point_index = (user_level << USER_LEVEL_SHIFT);

        fwrite(&entry_point_hash, sizeof(uint), 1, cfg_files->request_edge);
        fwrite(&entry_point_index, sizeof(uint), 1, cfg_files->request_edge);
        fwrite(&from_routine_hash, sizeof(uint), 1, cfg_files->request_edge);
        fwrite(&from_index, sizeof(uint), 1, cfg_files->request_edge);
      }

      request_state.is_new_request = false;
    }
  }

  if (!is_standalone_app) {
    if (!request_state.is_in_request)
      ERROR("Routine edge occurred outside of a request!\n");

    if (new_edge == NULL) {
      if (request_state.pool_index < request_state.edge_pool->size) {
        new_edge = scarray_get(request_state.edge_pool, request_state.pool_index++);
      } else {
        new_edge = PROCESS_NEW(request_edge_t);
        memset(new_edge, 0, sizeof(request_edge_t));
        scarray_append(request_state.edge_pool, new_edge);
        request_state.pool_index++;
      }
      new_edge->from_index = from_index;
      new_edge->from_routine_hash = from_routine_hash;
      new_edge->to_index = to_index;
      new_edge->to_routine_hash = to_routine_hash;
      new_edge->user_level = user_level;
      new_edge->next = edge_entry;
      sctable_add_or_replace(request_state.edges, key, new_edge);
    }

    if (IS_REQUEST_EDGE_OUTPUT_ENABLED()) {
      fwrite(&from_routine_hash, sizeof(uint), 1, cfg_files->request_edge);
      fwrite(&packed_from_index, sizeof(uint), 1, cfg_files->request_edge);
      fwrite(&to_routine_hash, sizeof(uint), 1, cfg_files->request_edge);
      fwrite(&to_index, sizeof(uint), 1, cfg_files->request_edge);
    }
  }

  return true;
}

void write_routine_edge(application_t *app, uint from_routine_hash, uint from_index,
                        uint to_routine_hash, uint to_index, user_level_t user_level)
{
  cfg_files_t *cfg_files = (cfg_files_t *) app->cfg_files;
  uint packed_from_index = from_index | (user_level << USER_LEVEL_SHIFT);

  fwrite(&from_routine_hash, sizeof(uint), 1, cfg_files->routine_edge);
  fwrite(&packed_from_index, sizeof(uint), 1, cfg_files->routine_edge);
  fwrite(&to_routine_hash, sizeof(uint), 1, cfg_files->routine_edge);
  fwrite(&to_index, sizeof(uint), 1, cfg_files->routine_edge);
}

/* text line: "<routine_hash> <unit_path>|<routine_name>" */
void write_routine_catalog_entry(application_t *app, uint routine_hash,
                                 const char *unit_path, const char *routine_name)
{
  cfg_files_t *cfg_files = (cfg_files_t *) app->cfg_files;

  fprintf(cfg_files->routine_catalog, "0x%x %s|%s\n", routine_hash,
          unit_path, routine_name);
}

void print_var_value(FILE *out, const zval *var)
{
  switch (var->u1.v.type) {
    case IS_UNDEF:
      fprintf(out, "const <undefined-type>");
      break;
    case IS_NULL:
      fprintf(out, "const null");
      break;
    case IS_FALSE:
      fprintf(out, "const false");
      break;
    case IS_TRUE:
      fprintf(out, "const true");
      break;
    case IS_LONG:
      fprintf(out, "const 0x%lx", var->value.lval);
      break;
    case IS_DOUBLE:
      fprintf(out, "const %f", var->value.dval);
      break;
    case IS_STRING: {
      fprintf(out, "\"%s\"", Z_STRVAL_P(var));
    } break;
    case IS_ARRAY:
      fprintf(out, "(array)");
      break;
    case IS_OBJECT:
      fprintf(out, "(object)");
      break;
    case IS_RESOURCE:
      fprintf(out, "(resource)");
      break;
    case IS_REFERENCE:
      fprintf(out, "reference? (zv:"PX")", p2int(var));
      break;
    default:
      fprintf(out, "what?? (zv:"PX")", p2int(var));
      break;
  }
}

void print_operand_value(FILE *out, const znode_op *operand)
{
  zend_execute_data *execute_data = EG(current_execute_data);
  print_var_value(out, EX_VAR(operand->var));
}

void print_operand(FILE *out, const char *tag, zend_op_array *ops,
                   const znode_op *operand, const zend_uchar type)
{
  fprintf(out, "%s ", tag);

  switch (type) {
    case IS_CONST:
      switch (operand->zv->u1.v.type) {
        case IS_UNDEF:
          fprintf(out, "const <undefined-type>");
          break;
        case IS_NULL:
          fprintf(out, "const null");
          break;
        case IS_FALSE:
          fprintf(out, "const false");
          break;
        case IS_TRUE:
          fprintf(out, "const true");
          break;
        case IS_LONG:
          fprintf(out, "const 0x%lx", operand->zv->value.lval);
          break;
        case IS_DOUBLE:
          fprintf(out, "const %f", operand->zv->value.dval);
          break;
        case IS_STRING: {
          uint i, j;
          char buffer[32] = {0};
          const char *str = Z_STRVAL_P(operand->zv);

          for (i = 0, j = 0; i < 31; i++) {
            if (str[i] == '\0')
              break;
            if (str[i] != '\n')
              buffer[j++] = str[i];
          }
          fprintf(out, "\"%s\"", buffer);
        } break;
        case IS_ARRAY:
          fprintf(out, "const array (zv:"PX")", p2int(operand->zv));
          break;
        case IS_OBJECT:
          fprintf(out, "const object? (zv:"PX")", p2int(operand->zv));
          break;
        case IS_RESOURCE:
          fprintf(out, "const resource? (zv:"PX")", p2int(operand->zv));
          break;
        case IS_REFERENCE:
          fprintf(out, "const reference? (zv:"PX")", p2int(operand->zv));
          break;
        default:
          fprintf(out, "const what?? (zv:"PX")", p2int(operand->zv));
          break;
      }
      break;
    case IS_VAR:
    case IS_TMP_VAR:
      fprintf(out, "var #%d: ", (uint) (EX_VAR_TO_NUM(operand->var) - ops->last_var));
      print_operand_value(out, operand);
      break;
    case IS_CV:
      fprintf(out, "var $%s: ", ops->vars[EX_VAR_TO_NUM(operand->var)]->val);
      print_operand_value(out, operand);
      break;
    case IS_UNUSED:
      fprintf(out, "-");
      break;
    case 0x24:
      fprintf(out, "(discarded)");
      break;
    default:
      fprintf(out, "? (type 0x%x)", type);
  }
  fprintf(out, " ");
}

#ifdef PLOG_TAINT
static void print_taint(FILE *out, taint_variable_t *taint)
{
  switch (taint->type) {
    case TAINT_TYPE_SITE_MOD: {
      site_modification_t *mod = (site_modification_t *) taint->taint;
      switch (mod->type) {
        case SITE_MOD_DB:
          fprintf(out, "<db-fetch> %s.%s", mod->db_table, mod->db_column);
          if (mod->db_value != NULL)
            fprintf(out, ": \"%.40s\"", mod->db_value);
          break;
        case SITE_MOD_FILE:
          fprintf(out, "<file-path> %s", mod->file_path);
          break;
        default: ;
      }
    } break;
# ifdef TAINT_REQUEST_INPUT
    case TAINT_TYPE_REQUEST_INPUT: {
      request_input_t *input = (request_input_t *) taint->taint;
      switch (input->type) {
        case REQUEST_INPUT_TYPE_REQUEST:
          fprintf(out, "<input-request> ");
          break;
        case REQUEST_INPUT_TYPE_GET:
          fprintf(out, "<input-get> ");
          break;
        case REQUEST_INPUT_TYPE_POST:
          fprintf(out, "<input-post> ");
          break;
        case REQUEST_INPUT_TYPE_COOKIE:
          fprintf(out, "<input-cookie> ");
          break;
        case REQUEST_INPUT_TYPE_SERVER:
          fprintf(out, "<input-server> ");
          break;
        case REQUEST_INPUT_TYPE_FILES:
          fprintf(out, "<input-files> ");
          break;
        default: return;
      }
      if (input->value == NULL)
        fprintf(out, "<dim-fetch>");
      else
        print_var_value(out, input->value);
    } break;
# endif
  }
}
#endif

#ifdef TAINT_IO
void plog_call__obsolete(application_t *app, const char *tag, const char *callee_name,
               zend_op_array *op_array, const zend_op *call_op,
               uint arg_count, const zend_op **args)
{
  uint i;
  FILE *plog = ((cfg_files_t *) app->cfg_files)->persistence;

  fprintf(plog, "%s at %s:%d\n", tag, op_array->filename->val, call_op->lineno);

  fprintf(plog, "%s %s ", tag, callee_name);
  for (i = 0; i < arg_count; i++)
    print_operand(plog, "<arg>", op_array, &args[i]->op1, args[i]->op1_type);
  print_operand(plog, "<ret>", op_array, &call_op->result, call_op->result_type);
  fprintf(plog, "\n");
}
#endif

#ifdef PLOG_TAINT
void plog_taint(application_t *app, taint_variable_t *taint_var)
{
  FILE *plog = ((cfg_files_t *) app->cfg_files)->persistence;
  print_taint(plog, taint_var);
}

void plog_taint_var(application_t *app, taint_variable_t *taint_var, uint64 hash)
{
  //bool plogged = true;
  FILE *plog = ((cfg_files_t *) app->cfg_files)->persistence;

  fprintf(plog, "\t<tainted-op> %s:%d (0x%llx)\n\t", taint_var->tainted_at_file,
          taint_var->tainted_at->lineno, hash);
  print_taint(plog, taint_var);

  /*
  switch (taint_var->var_type) {
    case TAINT_VAR_TEMP:
      fprintf(plog, "<taint-var> temp #%d with ", taint_var->var_id.temp_id);
      print_taint(plog, taint_var);
      break;
    case TAINT_VAR_LOCAL:
      fprintf(plog, "<taint-var> var #%d with ", taint_var->var_id.temp_id);
      print_taint(plog, taint_var);
      break;
    case TAINT_VAR_GLOBAL:
      fprintf(plog, "<taint-var> var %s with ", taint_var->var_id.var_name);
      print_taint(plog, taint_var);
      break;
    default:
      plogged = false;
  }
  if (plogged)
  */
    fprintf(plog, "\n");
}
#endif

void plog_call(zend_execute_data *execute_data, application_t *app, plog_type_t type,
               const char *callee_name, const zend_op **args, uint arg_count)
{
  uint i;
  const zval *arg_value;

  plog(app, type, "call %s(", callee_name);
  for (i = 0; i < arg_count; i++) {
    arg_value = get_arg_zval(execute_data, args[i]);
    switch (Z_TYPE_P(arg_value)) {
      case IS_UNDEF:
        plog_append(app, type, "?");
        break;
      case IS_NULL:
        plog_append(app, type, "null");
        break;
      case IS_TRUE:
        plog_append(app, type, "true");
        break;
      case IS_FALSE:
        plog_append(app, type, "false");
        break;
      case IS_STRING: {
        const char *str = Z_STRVAL_P(arg_value);
        uint len = strlen(str);

        if (len < 0x80 && strchr(str, '\n') == NULL && strchr(str, '\r') == NULL) {
          plog_append(app, type, "%s", str);
        } else {
          char buffer[20];
          uint i;

          if (len > 20)
            len = 20;

          for (i = 0; i < len; i++) {
            if (str[i] == '\n' || str[i] == '\r')
              buffer[i] = '$';
            else
              buffer[i] = str[i];
          }
          buffer[i] = '\0';
          plog_append(app, type, "%.20s", buffer);
        }
      } break;
      case IS_LONG:
        plog_append(app, type, "%d", Z_LVAL_P(arg_value));
        break;
      case IS_DOUBLE:
        plog_append(app, type, "%f", Z_DVAL_P(arg_value));
        break;
      case IS_ARRAY:
        plog_append(app, type, "<arr>");
        break;
      case IS_OBJECT:
        plog_append(app, type, "<obj>");
        break;
      case IS_RESOURCE: {
        const char *filename = get_resource_filename(arg_value);
        if (filename == NULL) {
          plog_append(app, type, "<res>");
        } else {
          plog_append(app, type, "file:%s", filename);
        }
      } break;
      case IS_REFERENCE:
        plog_append(app, type, "<ref>");
        break;
      case IS_CONSTANT:
        plog_append(app, type, "<const>");
        break;
      case IS_INDIRECT:
        plog_append(app, type, "<ind>");
        break;
      default:
        plog_append(app, type, "<%d>", Z_TYPE_P(arg_value));
        break;
    }
    if (i < (arg_count - 1))
      plog_append(app, type, ", ");
  }
  plog_append(app, type, ")\n");
}

static inline void plog_user_frame(application_t *app, plog_type_t type, zend_execute_data *frame)
{
  const char *routine_name;

  if (frame->func->op_array.function_name == NULL)
    routine_name = "<script-body>";
  else
    routine_name = frame->func->op_array.function_name->val;

  plog_append(app, type, "\t%04d(L%04d)%s:%s\n", frame->opline - frame->func->op_array.opcodes,
              frame->opline->lineno, site_relative_path(app, &frame->func->op_array), routine_name);
}

static inline void plog_internal_frame(application_t *app, plog_type_t type, zend_execute_data *frame)
{
  plog_append(app, type, "\t%s\n", frame->func->op_array.function_name->val);
}

void plog_stacktrace(application_t *app, plog_type_t type, zend_execute_data *start_frame)
{
  zend_execute_data *walk = start_frame;

  plog(app, type, "Stacktrace:\n");

  do {
    if (walk->func == NULL) {
      plog_append(app, type, "\t(empty)\n");
    } else {
      if (walk->func->op_array.type == ZEND_INTERNAL_FUNCTION)
        plog_internal_frame(app, type, walk);
      else
        plog_user_frame(app, type, walk);
    }
    walk = walk->prev_execute_data;
  } while (walk != NULL);
}

static inline bool is_plog_type_enabled(plog_type_t type)
{
#ifdef PLOG_TAINT
  if (type == PLOG_TYPE_TAINT)
    return true;
#endif
#ifdef PLOG_CFG
  if (type == PLOG_TYPE_CFG)
    return true;
#endif
#ifdef PLOG_DB
  if (type == PLOG_TYPE_DB)
    return true;
#endif
#ifdef PLOG_DB_MOD
  if (type == PLOG_TYPE_DB_MOD)
    return true;
#endif
#ifdef PLOG_FILE_MOD
  if (type == PLOG_TYPE_FILE_MOD)
    return true;
#endif
#ifdef PLOG_FILE_OUTPUT
  if (type == PLOG_TYPE_FILE_OUTPUT)
    return true;
#endif
#ifdef PLOG_SYS_WRITE
  if (type == PLOG_TYPE_SYS_WRITE)
    return true;
#endif
#ifdef PLOG_WARN
  if (type == PLOG_TYPE_WARN)
    return true;
#endif
#ifdef PLOG_AD_HOC
  if (type == PLOG_TYPE_AD_HOC)
    return true;
#endif
  return false;
}

static inline void plog_type_tag(FILE *plog, plog_type_t type)
{
  switch (type) {
    case PLOG_TYPE_TAINT:
      fprintf(plog, "<taint> ");
      break;
    case PLOG_TYPE_CFG:
      fprintf(plog, "<cfg> ");
      break;
    case PLOG_TYPE_DB:
      fprintf(plog, "<db> ");
      break;
    case PLOG_TYPE_DB_MOD:
      fprintf(plog, "<db-mod> ");
      break;
    case PLOG_TYPE_FILE_MOD:
      fprintf(plog, "<file-mod> ");
      break;
    case PLOG_TYPE_FILE_OUTPUT:
      fprintf(plog, "<file-out> ");
      break;
    case PLOG_TYPE_SYS_WRITE:
      fprintf(plog, "<sys-write> ");
      break;
    case PLOG_TYPE_WARN:
      fprintf(plog, "<warning> ");
      break;
    case PLOG_TYPE_AD_HOC:
      fprintf(plog, "<debug> ");
      break;
  }
}

void plog(application_t *app, plog_type_t type, const char *message, ...)
{
  if (is_plog_type_enabled(type)) {
    FILE *plog = ((cfg_files_t *) app->cfg_files)->persistence;

    plog_type_tag(plog, type);

    va_list args;
    va_start(args, message);

    vfprintf(plog, message, args);

    va_end(args);
  }
}

void plog_append(application_t *app, plog_type_t type, const char *message, ...)
{
  if (is_plog_type_enabled(type)) {
    FILE *plog = ((cfg_files_t *) app->cfg_files)->persistence;

    va_list args;
    va_start(args, message);

    vfprintf(plog, message, args);

    va_end(args);
  }
}

void flush_all_outputs(application_t *app)
{
  cfg_files_t *cfg_files = (cfg_files_t *) app->cfg_files;

  fflush(cfg_files->node);
  fflush(cfg_files->op_edge);
  fflush(cfg_files->routine_edge);
  fflush(cfg_files->routine_catalog);
  fflush(cfg_files->persistence);
  if (!is_standalone_app) {
    fflush(cfg_files->request);
    if (IS_REQUEST_EDGE_OUTPUT_ENABLED())
      fflush(cfg_files->request_edge);
  }
  fflush(stderr);
}

bool is_stateful_syscall(const char *callee_name)
{
  if (is_file_sink_function(callee_name))
    return true;

  if (strcmp(callee_name, "exec") == 0)
    return true;
  if (strcmp(callee_name, "shell_exec") == 0)
    return true;
  if (strcmp(callee_name, "system") == 0)
    return true;
  if (strcmp(callee_name, "proc_open") == 0)
    return true;

  return false;
}

int get_current_request_id()
{
  if (request_state.is_in_request)
    return request_state.request_id;
  else
    return -1;
}

uint64 get_current_request_start_time()
{
  return (uint64) request_state.r->request_time;
}

const char *get_current_request_address()
{
  return request_state.r->the_request;
}
