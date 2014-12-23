#include "php.h"
#include "lib/script_cfi_utils.h"
#include "cfg_handler.h"

static cfg_files_t cfg_files;

static inline void fnull(size_t size, FILE *file)
{
  static const char NULL_BUFFER[256] = {0};
  fwrite(NULL_BUFFER, 1, size, file);
}

static inline void fopcode(zend_uchar op)
{
  fwrite(&op, sizeof(zend_uchar), 1, cfg_files.node);
  fnull(3, cfg_files.node);
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

void starting_script(const char *script_path)
{
  uint index = 0;
  const char *script_filename;
  char *cfg_file_truncate;
  char cfg_file_path[256] = {0}, run_id[24] = {0};
  time_t timestamp;
  struct tm *calendar;
  struct stat dirinfo;

  time(&timestamp);
  calendar = localtime(&timestamp);
  
  strcat(cfg_file_path, OPMON_G(dataset_dir));
  index += strlen(cfg_file_path);
  
  if (script_path[strlen(script_path)-1] != '/')
    cfg_file_path[index++] = '/';
  
  strcat(cfg_file_path, "runs/");
  
  if (stat(cfg_file_path, &dirinfo) != 0)
    mkdir(cfg_file_path, 0700);

  script_filename = strrchr(script_path, '/');
  if (script_filename == NULL)
    script_filename = script_path;
  else
    script_filename++;
  strcat(cfg_file_path, script_filename);

  // .../script.2.1.php -> .../script.2.1
  cfg_file_truncate = strrchr(cfg_file_path, '.');
  if (cfg_file_truncate != NULL)
    *cfg_file_truncate = '\0';
  
  if (stat(cfg_file_path, &dirinfo) != 0)
    mkdir(cfg_file_path, 0700);
  
  sprintf(run_id, "/%d.%d-%d.%d/", 
          calendar->tm_yday, calendar->tm_hour, calendar->tm_min, getpid());
  strcat(cfg_file_path, run_id);

  PRINT("mkdir %s", cfg_file_path);
  mkdir(cfg_file_path, 0700);
  
  cfg_file_truncate = cfg_file_path + strlen(cfg_file_path);

#define OPEN_CFG_FILE(filename, file_field) \
  do { \
    *cfg_file_truncate = '\0'; \
    strcat(cfg_file_path, (filename)); \
    cfg_files.file_field = fopen(cfg_file_path, "w"); \
  } while (0);

  OPEN_CFG_FILE("/node.dat", node);
  OPEN_CFG_FILE("/op-edge.dat", op_edge);
  OPEN_CFG_FILE("/routine-edge.dat", routine_edge);
  
#undef OPEN_CFG_FILE
}

void write_node(uint unit_hash, uint function_hash, zend_uchar opcode)
{
  PRINT("Write node 0x%x 0x%x | 0x%01x to cfg\n", unit_hash, function_hash, opcode);
  fwrite(&unit_hash, sizeof(uint), 1, cfg_files.node);
  fwrite(&function_hash, sizeof(uint), 1, cfg_files.node);
  fopcode(opcode);
  fnull(sizeof(uint), cfg_files.node);
}

void write_op_edge(uint unit_hash, uint function_hash, uint from_index, uint to_index)
{
  PRINT("Write op-edge 0x%x 0x%x | 0x%x -> 0x%x to cfg\n", unit_hash, function_hash, from_index, to_index);
  
  fwrite(&unit_hash, sizeof(uint), 1, cfg_files.op_edge);
  fwrite(&function_hash, sizeof(uint), 1, cfg_files.op_edge);
  fwrite(&from_index, sizeof(uint), 1, cfg_files.op_edge);
  fwrite(&to_index, sizeof(uint), 1, cfg_files.op_edge);
}

void write_routine_edge(uint from_unit_hash, uint from_function_hash, uint from_index, 
                        uint to_unit_hash, uint to_function_hash, uint to_index)
{
  PRINT("Write routine-edge {0x%x 0x%x 0x%x} -> {0x%x 0x%x 0x%x} to cfg\n", 
        from_unit_hash, from_function_hash, from_index, 
        to_unit_hash, to_function_hash, to_index);
  
  fwrite(&from_unit_hash, sizeof(uint), 1, cfg_files.routine_edge);
  fwrite(&from_function_hash, sizeof(uint), 1, cfg_files.routine_edge);
  fwrite(&from_index, sizeof(uint), 1, cfg_files.routine_edge);
  fwrite(&to_unit_hash, sizeof(uint), 1, cfg_files.routine_edge);
  fwrite(&to_function_hash, sizeof(uint), 1, cfg_files.routine_edge);
  fwrite(&to_index, sizeof(uint), 1, cfg_files.routine_edge);
}
