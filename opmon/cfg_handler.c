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
  char *cfg_file_extension;
  char cfg_file_path[256], cfg_file_basename[256] = {0};
  
  strcat(cfg_file_basename, OPMON_G(dataset_dir));
  index += strlen(cfg_file_basename);
  
  if (script_path[strlen(script_path)-1] != '/')
    cfg_file_basename[index++] = '/';

  script_filename = strrchr(script_path, '/');
  if (script_filename == NULL)
    script_filename = script_path;
  else
    script_filename++;
  strcat(cfg_file_basename, script_filename);
  
  cfg_file_extension = strrchr(cfg_file_basename, '.');
  if (cfg_file_extension != NULL)
    *cfg_file_extension = '\0';
  
  sprintf(cfg_file_path, "%s.%s.dat", cfg_file_basename, "node");
  cfg_files.node = fopen(cfg_file_path, "w");
  sprintf(cfg_file_path, "%s.%s.dat", cfg_file_basename, "op-edge");
  cfg_files.op_edge = fopen(cfg_file_path, "w");
  sprintf(cfg_file_path, "%s.%s.dat", cfg_file_basename, "routine-edge");
  cfg_files.routine_edge = fopen(cfg_file_path, "w");
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
