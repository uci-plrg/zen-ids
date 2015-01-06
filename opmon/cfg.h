#ifndef _CFG_H_
#define _CFG_H_ 1

#include "php.h"
#include "lib/script_cfi_array.h"
#include "lib/script_cfi_utils.h"

typedef struct _cfg_node_t {
  zend_uchar opcode;
  uint index;
} cfg_node_t;

typedef struct _cfg_opcode_edge_t {
  uint from_index;
  uint to_index;
} cfg_opcode_edge_t;

typedef struct _cfg_opcode_t {
  zend_uchar opcode;
  byte extended_value;
} cfg_opcode_t;

typedef struct _routine_cfg_t {
  uint unit_hash;
  uint routine_hash;
  uint opcode_count;
  uint edge_count;
  cfg_opcode_t opcodes[256];
  cfg_opcode_edge_t edges[256];
} routine_cfg_t;

typedef struct _cfg_routine_edge_t {
  cfg_node_t from_node;
  routine_cfg_t *from_routine;
  routine_cfg_t *to_routine;
} cfg_routine_edge_t;

typedef struct _cfg_t {
  uint routine_count;
  uint edge_count;
  routine_cfg_t *routines[256];
  cfg_routine_edge_t edges[256];
} cfg_t;

routine_cfg_t *routine_cfg_new(uint unit_hash, uint routine_hash);
void routine_cfg_assign_opcode(routine_cfg_t *cfg, zend_uchar opcode, 
                               zend_uchar extended_value, uint index);
void routine_cfg_add_edge(routine_cfg_t *cfg, uint from_index, uint to_index);

cfg_t *cfg_new();
void cfg_add_routine(cfg_t *cfg, routine_cfg_t *routine);
void cfg_add_routine_edge(cfg_t *cfg, cfg_node_t from_node, 
                          routine_cfg_t *from_routine, routine_cfg_t *to_routine);

#endif
