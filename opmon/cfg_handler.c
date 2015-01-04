#include "php.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "lib/script_cfi_utils.h"
#include "cfg_handler.h"

static cfg_files_t cfg_files;

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
  PRINT("  === Close cfg file\n");
  
  if (cfg_files.node != NULL)  {
    fclose(cfg_files.node);
    fclose(cfg_files.op_edge);
    fclose(cfg_files.routine_edge);
  }
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
  
  if (stat(cfg_file_path, &dirinfo) != 0)
    mkdir(cfg_file_path, 0700);
  
  sprintf(run_id, "/%d.%d-%d.%d/", 
          calendar->tm_yday, calendar->tm_hour, calendar->tm_min, getpid());
  strcat(cfg_file_path, run_id);

  mkdir(cfg_file_path, 0700);
  
  cfg_file_truncate = cfg_file_path + strlen(cfg_file_path);

#define OPEN_CFG_FILE(filename, file_field) \
  do { \
    *cfg_file_truncate = '\0'; \
    strcat(cfg_file_path, (filename)); \
    cfg_files.file_field = fopen(cfg_file_path, "w"); \
  } while (0);

  OPEN_CFG_FILE("/node.run", node);
  OPEN_CFG_FILE("/op-edge.run", op_edge);
  OPEN_CFG_FILE("/routine-edge.run", routine_edge);
  
#undef OPEN_CFG_FILE
}

void starting_script(const char *script_path)
{
  open_output_files(script_path);
  load_dataset(script_path);
}

void write_node(uint unit_hash, uint routine_hash, cfg_opcode_t *opcode, uint index)
{
  PRINT("Write node 0x%x 0x%x | 0x%01x to cfg\n", unit_hash, routine_hash, opcode->opcode);
  fwrite(&unit_hash, sizeof(uint), 1, cfg_files.node);
  fwrite(&routine_hash, sizeof(uint), 1, cfg_files.node);
  fopcode(opcode, cfg_files.node);
  fwrite(&index, sizeof(uint), 1, cfg_files.node);
}

void write_op_edge(uint unit_hash, uint routine_hash, uint from_index, uint to_index)
{
  PRINT("Write op-edge 0x%x 0x%x | 0x%x -> 0x%x to cfg\n", unit_hash, routine_hash, from_index, to_index);
  
  fwrite(&unit_hash, sizeof(uint), 1, cfg_files.op_edge);
  fwrite(&routine_hash, sizeof(uint), 1, cfg_files.op_edge);
  fwrite(&from_index, sizeof(uint), 1, cfg_files.op_edge);
  fwrite(&to_index, sizeof(uint), 1, cfg_files.op_edge);
}

void write_routine_edge(uint from_unit_hash, uint from_routine_hash, uint from_index, 
                        uint to_unit_hash, uint to_routine_hash, uint to_index)
{
  PRINT("Write routine-edge {0x%x 0x%x 0x%x} -> {0x%x 0x%x 0x%x} to cfg\n", 
        from_unit_hash, from_routine_hash, from_index, 
        to_unit_hash, to_routine_hash, to_index);
  
  fwrite(&from_unit_hash, sizeof(uint), 1, cfg_files.routine_edge);
  fwrite(&from_routine_hash, sizeof(uint), 1, cfg_files.routine_edge);
  fwrite(&from_index, sizeof(uint), 1, cfg_files.routine_edge);
  fwrite(&to_unit_hash, sizeof(uint), 1, cfg_files.routine_edge);
  fwrite(&to_routine_hash, sizeof(uint), 1, cfg_files.routine_edge);
  fwrite(&to_index, sizeof(uint), 1, cfg_files.routine_edge);
}
