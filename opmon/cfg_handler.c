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
#include "operand_resolver.h"
#include "cfg_handler.h"

// #define URL_DECODE 1

typedef struct _cfg_files_t {
  FILE *node;
  FILE *op_edge;
  FILE *routine_edge;
  FILE *request;
  FILE *request_edge;
  FILE *routine_catalog;
} cfg_files_t;

static char session_file_path[256] = {0}, cfg_file_path[256] = {0};

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

typedef struct _php_server_context_t { // type window
  int opaque;
  request_rec *r;
  apr_bucket_brigade *bb;
} php_server_context_t;

typedef struct _request_state_t {
  bool is_in_request;
  bool is_new_request;
  uint request_id;
  application_t *app;
  request_rec *r;
  sctable_t *edges; // of `request_edge_t`
  uint pool_index;
  scarray_t *edge_pool;
} request_state_t;

static request_state_t request_state;
static const uint request_header_tag = 2; // N.B.: avoid collision with app entry point hash
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

static void write_request_entry(cfg_files_t *cfg_files)
{
  fprintf(cfg_files->request, "<client-ip> %s\n", request_state.r->useragent_ip);
  fprintf(cfg_files->request, "<request> %s\n", request_state.r->the_request);
  fprintf(cfg_files->request, "<requested-file> %s\n", request_state.r->filename);
  fprintf(cfg_files->request, "<protocol> %s\n", request_state.r->protocol);
  fprintf(cfg_files->request, "<method> %s\n", request_state.r->method);
  fprintf(cfg_files->request, "<content-type> %s\n", request_state.r->content_type);
  fprintf(cfg_files->request, "<content-encodings> %s\n", request_state.r->content_encoding);
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
      apr_size_t size;
#ifdef URL_DECODE
      char decode_buffer[SAPI_POST_BLOCK_SIZE];
      apr_size_t decode_size, decode_fragment_size = 0;

      decode_buffer[0] = '\0';
#endif

      fprintf(cfg_files->request, "<post-variable> ");
      php_stream_rewind(SG(request_info).request_body);
      while (true) {
        size = SG(request_info).request_body->ops->read(SG(request_info).request_body, buffer,
                                                        SAPI_POST_BLOCK_SIZE);
        if (size == 0)
          break;
        set = buffer;
        do {
          mark = strchr(set, '&');
          if (mark == NULL)
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
        fwrite(set, sizeof(char), size - (set - buffer), cfg_files->request);
#endif
      }
      fprintf(cfg_files->request, "\n");
    } break;
    default:
      ERROR("Unknown request type %s\n", request_state.r->method);
  }

  fprintf(cfg_files->request, "<request-id> |%08d\n", request_state.request_id);
}

void init_cfg_handler()
{
  memset(&request_state, 0, sizeof(request_state_t));
}

void close_cfg_files(application_t *app)
{
  cfg_files_t *cfg_files = (cfg_files_t *) app->cfg_files;

  PRINT("  === Close cfg files\n");

  if (cfg_files != NULL)  {
    fclose(cfg_files->node);
    fclose(cfg_files->op_edge);
    fclose(cfg_files->routine_edge);
    fclose(cfg_files->routine_catalog);
    if (!is_standalone_app) {
      fclose(cfg_files->request);
      fclose(cfg_files->request_edge);
    }
  }

  if (request_state.edges != NULL) {
    request_edge_t *edge;
    for (edge = (request_edge_t *) scarray_iterator_start(request_state.edge_pool);
         edge != NULL; edge = (request_edge_t *) scarray_iterator_next()) {
      free(edge);
    }
    scarray_destroy(request_state.edge_pool);
    free(request_state.edge_pool);
    request_state.edge_pool = NULL;
    sctable_destroy(request_state.edges);
    free(request_state.edges);
    request_state.edges = NULL;
  }
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
  if (!is_standalone_app) {
    OPEN_CFG_FILE("request.tab", request);
    OPEN_CFG_FILE("request-edge.run", request_edge);
  }

#undef OPEN_CFG_FILE
}

static void open_output_files(const char *script_path)
{
  char cfg_file_path[256] = {0}, run_id[24] = {0};
  struct stat dirinfo;
  time_t timestamp;
  struct tm *calendar;

  time(&timestamp);
  calendar = localtime(&timestamp);

  setup_base_path(cfg_file_path, "runs", script_path);

  if (stat(cfg_file_path, &dirinfo) != 0) {
    if (mkdir(cfg_file_path, 0777) != 0) {
      ERROR("Failed to create the cfg file directory %s\n", cfg_file_path);
      return;
    }
  }

  sprintf(run_id, "/%d.%d-%d.%d/",
          calendar->tm_yday, calendar->tm_hour, calendar->tm_min, getpid());
  strcat(cfg_file_path, run_id);

  if (mkdir(cfg_file_path, 0777) != 0) {
    ERROR("Failed to create the cfg file directory %s\n", cfg_file_path);
    return;
  }

  open_output_files_in_dir(&standalone_cfg_files, cfg_file_path, "w");
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

    app->cfg_files = malloc(sizeof(cfg_files_t));
    open_output_files_in_dir((cfg_files_t *) app->cfg_files, app_cfg_file_path, "w");

    app->dataset = load_dataset(app->name);
  }

  initialize_interp_app_context(app);
}

void cfg_request(bool start)
{
  request_state.is_in_request = start;

  if (request_state.is_in_request) {
    php_server_context_t *context = (php_server_context_t *) SG(server_context);
    request_state.r = context->r;
    request_state.is_new_request = true;
    request_state.request_id++;

    if (request_state.edges == NULL) { // lazy construct b/c standalone mode doesn't need it
      request_state.edge_pool = malloc(sizeof(scarray_t));
      scarray_init(request_state.edge_pool);
      request_state.edges = malloc(sizeof(sctable_t));
      request_state.edges->hash_bits = 10;
      sctable_init(request_state.edges);
    } else { // clear the request edges
      sctable_clear(request_state.edges);
      request_state.pool_index = 0;
    }

    if (PS(id) != NULL && strcmp(PS(id)->val, session.id) != 0) {
      strcpy(session.id, PS(id)->val);
      session.hash = hash_string(session.id);
      SPOT("New session id %s (0x%x)\n", session.id, session.hash);
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

  from_index |= (user_level << 26);

  fwrite(&routine_hash, sizeof(uint), 1, cfg_files->op_edge);
  fwrite(&from_index, sizeof(uint), 1, cfg_files->op_edge);
  fwrite(&to_index, sizeof(uint), 1, cfg_files->op_edge);
}

/* 4 x 4 bytes: { from_routine_hash | user_level (6) from_index (26) |
 *                to_routine_hash | to_index }
 */
void write_routine_edge(bool is_new_in_process, application_t *app, uint from_routine_hash,
                        uint from_index, uint to_routine_hash, uint to_index,
                        user_level_t user_level)
{
  cfg_files_t *cfg_files = (cfg_files_t *) app->cfg_files;
  request_edge_t *edge_entry = NULL, *new_edge = NULL;
  uint64 key = HASH_ROUTINE_EDGE(from_routine_hash, to_routine_hash);
  uint packed_from_index;

  if (!is_new_in_process) {
    request_edge_t *next = (request_edge_t *) sctable_lookup(request_state.edges, key);
    edge_entry = next;
    while (next != NULL) {
      if (from_routine_hash == next->from_routine_hash && from_index == next->from_index &&
          to_routine_hash == next->to_routine_hash && to_index == next->to_index) {
        new_edge = next;
        if (new_edge->user_level <= user_level)
          return; // nothing new about this edge
        new_edge->user_level = user_level;
        break;
      }
      next = next->next;
    }
  }

  PRINT("Write routine-edge {0x%x #%d} -> {0x%x #%d} to cfg\n",
        from_routine_hash, from_index, to_routine_hash, to_index);

  packed_from_index = from_index | (user_level << 26);

  if (request_state.is_new_request) {
    uint timestamp = get_timestamp();

    fwrite(&request_header_tag, sizeof(uint), 1, cfg_files->request_edge);
    fwrite(&request_state.request_id, sizeof(uint), 1, cfg_files->request_edge);
    fwrite(&session.hash, sizeof(uint), 1, cfg_files->request_edge);
    fwrite(&timestamp, sizeof(uint), 1, cfg_files->request_edge);

    write_request_entry(cfg_files);

    request_state.is_new_request = false;
    request_state.app = app;
  }
  if (is_new_in_process) {
    fwrite(&from_routine_hash, sizeof(uint), 1, cfg_files->routine_edge);
    fwrite(&packed_from_index, sizeof(uint), 1, cfg_files->routine_edge);
    fwrite(&to_routine_hash, sizeof(uint), 1, cfg_files->routine_edge);
    fwrite(&to_index, sizeof(uint), 1, cfg_files->routine_edge);
  }
  if (!is_standalone_app) {
    if (!request_state.is_in_request)
      ERROR("Routine edge occurred outside of a request!\n");

    if (new_edge == NULL) {
      if (request_state.pool_index < request_state.edge_pool->size) {
        new_edge = scarray_get(request_state.edge_pool, request_state.pool_index++);
        if (new_edge == edge_entry)
          ERROR("Fail!\n");
      } else {
        new_edge = malloc(sizeof(request_edge_t));
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

    fwrite(&from_routine_hash, sizeof(uint), 1, cfg_files->request_edge);
    fwrite(&packed_from_index, sizeof(uint), 1, cfg_files->request_edge);
    fwrite(&to_routine_hash, sizeof(uint), 1, cfg_files->request_edge);
    fwrite(&to_index, sizeof(uint), 1, cfg_files->request_edge);
  }
}

/* text line: "<routine_hash> <unit_path>|<routine_name>" */
void write_routine_catalog_entry(application_t *app, uint routine_hash,
                                 const char *unit_path, const char *routine_name)
{
  cfg_files_t *cfg_files = (cfg_files_t *) app->cfg_files;

  fprintf(cfg_files->routine_catalog, "0x%x %s|%s\n", routine_hash,
          unit_path, routine_name);
}

void flush_all_outputs(application_t *app)
{
  cfg_files_t *cfg_files = (cfg_files_t *) app->cfg_files;

  fflush(cfg_files->node);
  fflush(cfg_files->op_edge);
  fflush(cfg_files->routine_edge);
  fflush(cfg_files->routine_catalog);
  if (!is_standalone_app) {
    fflush(cfg_files->request);
    fflush(cfg_files->request_edge);
  }
  fflush(stderr);
}
