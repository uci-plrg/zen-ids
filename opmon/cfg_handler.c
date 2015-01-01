#include "php.h"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "lib/script_cfi_utils.h"
#include "cfg_handler.h"

static cfg_files_t cfg_files;

static size_t dataset_size;
static void *dataset;

static inline void fnull(size_t size, FILE *file)
{
  static const char NULL_BUFFER[256] = {0};
  fwrite(NULL_BUFFER, 1, size, file);
}

static inline void fopcode(zend_uchar op, FILE *file)
{
  fwrite(&op, sizeof(zend_uchar), 1, file);
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
  
  if (dataset != NULL)
    munmap(dataset, dataset_size);
}

static void setup_base_path(char *path, const char *category, const char *script_path)
{
  char *path_truncate;
  const char *script_filename;
  struct stat dirinfo;
  
  strcat(path, OPMON_G(dataset_dir));
  
  uint len = strlen(path);
  if (path[len-1] != '/')
    path[len] = '/';
  
  strcat(path, category);
  strcat(path, "/");
  
  if (stat(path, &dirinfo) != 0)
    mkdir(path, 0700);

  script_filename = strrchr(script_path, '/');
  if (script_filename == NULL)
    script_filename = script_path;
  else
    script_filename++;
  strcat(path, script_filename);

  // .../script.2.1.php -> .../script.2.1
  path_truncate = strrchr(path, '.');
  if (path_truncate != NULL)
    *path_truncate = '\0';
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

void load_dataset(const char *script_path)
{
  char dataset_path[256] = {0};
  struct stat fileinfo;
  int dataset_file;
  
  setup_base_path(dataset_path, "sets", script_path);
  strcat(dataset_path, ".set");
  
  if (stat(dataset_path, &fileinfo) != 0) {
    PRINT("Failed to obtain file info for path %s. Skipping dataset operations.\n", dataset_path);
    dataset = NULL;
    return;
  }
  dataset_size = fileinfo.st_size;
  
  dataset_file = open(dataset_path, O_RDONLY);
  if (dataset_file == -1) {
    PRINT("Failed to open the dataset at path %s. Skipping dataset operations.\n", dataset_path);
    dataset = NULL;
    return;
  }
  
  dataset = mmap(NULL, dataset_size, PROT_READ, MAP_SHARED, dataset_file, 0);
  PRINT("Mapped %d bytes from dataset at path %s.\n", (int) dataset_size, dataset_path);
}

void starting_script(const char *script_path)
{
  open_output_files(script_path);
  load_dataset(script_path);
}

void write_node(uint unit_hash, uint function_hash, zend_uchar opcode, uint index)
{
  PRINT("Write node 0x%x 0x%x | 0x%01x to cfg\n", unit_hash, function_hash, opcode);
  fwrite(&unit_hash, sizeof(uint), 1, cfg_files.node);
  fwrite(&function_hash, sizeof(uint), 1, cfg_files.node);
  fopcode(opcode, cfg_files.node);
  fwrite(&index, sizeof(uint), 1, cfg_files.node);
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
