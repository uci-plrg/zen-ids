#ifndef _CFG_H_
#define _CFG_H_ 1

#include "php.h"

typedef struct _cfg_node_t {
  zend_uchar opcode;
  uint index;
} cfg_node_t;

typedef struct _cfg_opcode_edge_t {
  uint from_index;
  uint to_index;
} cfg_opcode_edge_t;

typedef struct _routine_cfg_t {
  uint unit_hash;
  uint function_hash;
  uint opcode_count;
  uint edge_count;
  zend_uchar opcodes[256];
  cfg_opcode_edge_t edges[256];
} routine_cfg_t;

typedef struct _cfg_routine_edge_t {
  cfg_node_t from_node;
  routine_cfg_t from_routine;
  routine_cfg_t to_routine;
} cfg_routine_edge_t;

typedef struct _cfg_t {
  routine_cfg_t routines[1]; // *
  cfg_routine_edge_t edges[1]; // *
} cfg_t;

routine_cfg_t *routine_cfg_new(uint unit_hash, uint function_hash);
void routine_cfg_add_node(routine_cfg_t *cfg, zend_uchar opcode);
void routine_cfg_add_edge(routine_cfg_t *cfg, uint from_index, uint to_index);

#endif
