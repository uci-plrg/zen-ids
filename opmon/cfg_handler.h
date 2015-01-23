#ifndef _CFG_HANDLER_H_ 
#define _CFG_HANDLER_H_ 1

#include "cfg.h"

typedef struct _cfg_files_t {
  FILE *node;
  FILE *op_edge;
  FILE *routine_edge;
} cfg_files_t;

void init_cfg_handler();
void destroy_cfg_handler();

void starting_script(const char *script_path);
void write_node(uint unit_hash, uint routine_hash, cfg_opcode_t *opcode, uint index);
void write_op_edge(uint unit_hash, uint routine_hash, uint from_index, uint to_index);
void write_routine_edge(uint from_unit_hash, uint from_routine_hash, uint from_index, 
                        uint to_unit_hash, uint to_routine_hash, uint to_index);

void flush_all_outputs();

#endif
