#ifndef _CFG_HANDLER_H_ 
#define _CFG_HANDLER_H_ 1

typedef struct _cfg_files_t {
  FILE *node;
  FILE *op_edge;
  FILE *routine_edge;
} cfg_files_t;

void init_cfg_handler();
void destroy_cfg_handler();

void starting_script(const char *script_path);
void write_node(uint unit_hash, uint function_hash, zend_uchar opcode, uint index);
void write_op_edge(uint unit_hash, uint function_hash, uint from_index, uint to_index);
void write_routine_edge(uint from_unit_hash, uint from_function_hash, uint from_index, 
                        uint to_unit_hash, uint to_function_hash, uint to_index);

#endif
