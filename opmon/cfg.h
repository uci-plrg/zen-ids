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
  user_level_t user_level;
} cfg_opcode_edge_t;

typedef struct _cfg_opcode_t {
  zend_uchar opcode;
  byte extended_value;
  ushort line_number;
} cfg_opcode_t;

typedef struct _routine_cfg_t {
  uint routine_hash;
  scarray_t opcodes;        // cfg_opcode_t *
  scarray_t opcode_edges;   // cfg_opcode_edge_t *
  scarray_t routine_edges;  // cfg_routine_edge_t *
} routine_cfg_t;

// todo: add type { call, exception } ?
typedef struct _cfg_routine_edge_t {
  uint from_index;
  uint to_index;
  user_level_t user_level;
  routine_cfg_t *from_routine;
  routine_cfg_t *to_routine;
} cfg_routine_edge_t;

typedef struct _cfg_t {
  scarray_t routines; // routine_cfg_t * // TODO: shouldn't I be a hashtable?
} cfg_t;

static inline bool
is_same_routine_cfg(routine_cfg_t *first, routine_cfg_t *second)
{
  return first->routine_hash == second->routine_hash;
}

static inline routine_cfg_t *
cfg_get_routine(cfg_t *cfg, uint index)
{
  return (routine_cfg_t *) scarray_get(&cfg->routines, index);
}

static inline routine_cfg_t *
cfg_routine_lookup(cfg_t *cfg, uint routine_hash)
{
  uint i;
  for (i = 0; i < cfg->routines.size; i++) {
    routine_cfg_t *routine = cfg_get_routine(cfg, i);
    if (routine->routine_hash == routine_hash)
      return routine;
  }
  return NULL;
}

static inline cfg_opcode_t *
routine_cfg_get_opcode(routine_cfg_t *routine, uint index)
{
  return (cfg_opcode_t *) scarray_get(&routine->opcodes, index);
}

static inline cfg_opcode_edge_t *
routine_cfg_get_opcode_edge(routine_cfg_t *routine, uint index)
{
  return (cfg_opcode_edge_t *) scarray_get(&routine->opcode_edges, index);
}

static inline cfg_routine_edge_t *
routine_cfg_get_routine_edge(routine_cfg_t *routine, uint index)
{
  return (cfg_routine_edge_t *) scarray_get(&routine->routine_edges, index);
}

routine_cfg_t *routine_cfg_new(uint routine_hash);
void routine_cfg_assign_opcode(routine_cfg_t *cfg, zend_uchar opcode, zend_uchar extended_value,
                               ushort line_number, uint index);
bool routine_cfg_has_opcode_edge(routine_cfg_t *cfg, uint from_index, uint to_index);
void routine_cfg_add_opcode_edge(routine_cfg_t *cfg, uint from_index, uint to_index,
                                 user_level_t user_level);

cfg_t *cfg_new();
void cfg_add_routine(cfg_t *cfg, routine_cfg_t *routine);
bool cfg_has_routine_edge(routine_cfg_t *from_routine, uint from_index,
                          routine_cfg_t *to_routine, uint to_index);
void cfg_add_routine_edge(routine_cfg_t *from_routine, uint from_index,
                          routine_cfg_t *to_routine, uint to_index,
                          user_level_t user_level);

#endif
