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
  scarray_t opcodes; // cfg_opcode_t *
  scarray_t edges;   // cfg_opcode_edge_t *
} routine_cfg_t;

typedef struct _cfg_routine_edge_t {
  cfg_node_t from_node;
  routine_cfg_t *from_routine;
  routine_cfg_t *to_routine;
} cfg_routine_edge_t;

typedef struct _cfg_t {
  scarray_t routines; // routine_cfg_t *
  scarray_t edges;    // routine_edge_t *
} cfg_t;

static inline routine_cfg_t *cfg_get_routine(cfg_t *cfg, uint index)
{
  return (routine_cfg_t *) scarray_get(&cfg->routines, index);
}

static inline cfg_opcode_t *routine_cfg_get_opcode(routine_cfg_t *routine, uint index)
{
  return (cfg_opcode_t *) scarray_get(&routine->opcodes, index);
}

static inline cfg_opcode_edge_t *routine_cfg_get_edge(routine_cfg_t *routine, uint index)
{
  return (cfg_opcode_edge_t *) scarray_get(&routine->edges, index);
}

routine_cfg_t *routine_cfg_new(uint unit_hash, uint routine_hash);
void routine_cfg_assign_opcode(routine_cfg_t *cfg, zend_uchar opcode, 
                               zend_uchar extended_value, uint index);
void routine_cfg_add_edge(routine_cfg_t *cfg, uint from_index, uint to_index);

cfg_t *cfg_new();
void cfg_add_routine(cfg_t *cfg, routine_cfg_t *routine);
void cfg_add_routine_edge(cfg_t *cfg, cfg_node_t from_node, 
                          routine_cfg_t *from_routine, routine_cfg_t *to_routine);

#endif
