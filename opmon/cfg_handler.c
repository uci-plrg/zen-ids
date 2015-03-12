#include "php.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "lib/script_cfi_utils.h"
#include "cfg_handler.h"

static cfg_files_t cfg_files;
static char session_file_path[256] = {0};

static inline void fnull(size_t size, FILE *file)
{
  static const char NULL_BUFFER[256] = {0};
  fwrite(NULL_BUFFER, 1, size, file);
}

static inline void fopcode(cfg_opcode_t *opcode, FILE *file)
{
  fwrite(&opcode->opcode, sizeof(zend_uchar), 1, file);
  if (opcode->opcode == ZEND_INCLUDE_OR_EVAL) {
    fwrite(&opcode->extended_value, sizeof(zend_uchar), 1, file);
    fnull(2, file);
  } else {
    fnull(3, file);
  }
}

void init_cfg_handler()
{
  cfg_files.node = NULL;
}

void destroy_cfg_handler()
{
  PRINT("  === Close cfg files\n");

  if (cfg_files.node != NULL)  {
    fclose(cfg_files.node);
    fclose(cfg_files.op_edge);
    fclose(cfg_files.routine_edge);
    fclose(cfg_files.routine_catalog);
  }
}

static void open_output_files_in_dir(char *cfg_file_path)
{
  char *cfg_file_truncate = cfg_file_path + strlen(cfg_file_path);

  SPOT("Attempting to open output files in dir %s\n", cfg_file_path);

#define OPEN_CFG_FILE(filename, file_field) \
  do { \
    *cfg_file_truncate = '\0'; \
    strcat(cfg_file_path, (filename)); \
    cfg_files.file_field = fopen(cfg_file_path, "w"); \
    if (cfg_files.file_field == NULL) \
      ERROR("Failed to open cfg file cfg_files."#file_field"\n"); \
    else \
      SPOT("Successfully opened cfg_files."#file_field"\n"); \
  } while (0);

  OPEN_CFG_FILE("node.run", node);
  OPEN_CFG_FILE("op-edge.run", op_edge);
  OPEN_CFG_FILE("routine-edge.run", routine_edge);
  OPEN_CFG_FILE("routine-catalog.tab", routine_catalog);

#undef OPEN_CFG_FILE
}

static void open_output_files(const char *script_path)
{
  char cfg_file_path[256] = {0}, run_id[24] = {0};
  char *cfg_file_truncate;
  struct stat dirinfo;
  time_t timestamp;
  struct tm *calendar;

  time(&timestamp);
  calendar = localtime(&timestamp);

  setup_base_path(cfg_file_path, "runs", script_path);

  if (stat(cfg_file_path, &dirinfo) != 0) {
    if (mkdir(cfg_file_path, 0700) != 0) {
      ERROR("Failed to create the cfg file directory %s\n", cfg_file_path);
      return;
    }
  }

  sprintf(run_id, "/%d.%d-%d.%d/",
          calendar->tm_yday, calendar->tm_hour, calendar->tm_min, getpid());
  strcat(cfg_file_path, run_id);

  if (mkdir(cfg_file_path, 0700) != 0) {
    ERROR("Failed to create the cfg file directory %s\n", cfg_file_path);
    return;
  }

  open_output_files_in_dir(cfg_file_path);
}

void starting_script(const char *script_path)
{
  char *resolved_path = zend_resolve_path(script_path, strlen(script_path));

  SPOT("starting_script %s on pid %d\n", resolved_path, getpid());

  open_output_files(resolved_path);
  load_dataset(resolved_path);

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

  load_dataset("webserver");
}

void worker_startup()
{
  char cfg_file_path[256] = {0}, session_id[24] = {0};
  struct stat dirinfo;
  FILE *session_dir;

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

  open_output_files_in_dir(cfg_file_path);
}

/* 3 x 4 bytes: { routine_hash | opcode | index } */
void write_node(uint routine_hash, cfg_opcode_t *opcode, uint index)
{
  PRINT("Write node 0x%x #%d 0x%01x to cfg\n", routine_hash, index, opcode->opcode);
  fwrite(&routine_hash, sizeof(uint), 1, cfg_files.node);
  fopcode(opcode, cfg_files.node);
  fwrite(&index, sizeof(uint), 1, cfg_files.node);
}

/* 3 x 4 bytes: { routine_hash | user_level (6) from_index (26) | to_index } */
void write_op_edge(uint routine_hash, uint from_index, uint to_index,
                   user_level_t user_level)
{
  PRINT("Write op-edge 0x%x #%d -> #%d to cfg\n", routine_hash, from_index, to_index);

  from_index |= (user_level << 26);

  fwrite(&routine_hash, sizeof(uint), 1, cfg_files.op_edge);
  fwrite(&from_index, sizeof(uint), 1, cfg_files.op_edge);
  fwrite(&to_index, sizeof(uint), 1, cfg_files.op_edge);
}

/* 4 x 4 bytes: { from_routine_hash | user_level (6) from_index (26) |
 *                to_routine_hash | to_index }
 */
void write_routine_edge(uint from_routine_hash, uint from_index,
                        uint to_routine_hash, uint to_index,
                        user_level_t user_level)
{
  PRINT("Write routine-edge {0x%x #%d} -> {0x%x #%d} to cfg\n",
        from_routine_hash, from_index, to_routine_hash, to_index);

  from_index |= (user_level << 26);

  fwrite(&from_routine_hash, sizeof(uint), 1, cfg_files.routine_edge);
  fwrite(&from_index, sizeof(uint), 1, cfg_files.routine_edge);
  fwrite(&to_routine_hash, sizeof(uint), 1, cfg_files.routine_edge);
  fwrite(&to_index, sizeof(uint), 1, cfg_files.routine_edge);
}

/* text line: "<routine_hash> <unit_path>|<routine_name>" */
void write_routine_catalog_entry(uint routine_hash,
                                 const char *unit_path, const char *routine_name)
{
  fprintf(cfg_files.routine_catalog, "0x%x %s|%s\n", routine_hash,
          unit_path, routine_name);
}

void flush_all_outputs()
{
  fflush(cfg_files.node);
  fflush(cfg_files.op_edge);
  fflush(cfg_files.routine_edge);
  fflush(cfg_files.routine_catalog);
  fflush(stderr);
}
