#ifndef _CFG_H_
#define _CFG_H_ 1

#include "php.h"
#include "lib/script_cfi_hashtable.h"
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
  sctable_t routines;       // routine_cfg_t * // TODO: shouldn't I be a hashtable?
  sctable_t routine_edges;  // cfg_routine_edge_t *
} cfg_t;

typedef struct _application_t {
  const char *name;
  const char *root; // ends in '/'
  cfg_t *cfg;
  void *cfg_files;
  void *dataset;
  void *base_frame;
} application_t;

static inline bool
is_same_routine_cfg(routine_cfg_t *first, routine_cfg_t *second)
{
  return first->routine_hash == second->routine_hash;
}

static inline routine_cfg_t *
cfg_routine_lookup(cfg_t *cfg, uint routine_hash)
{
  return sctable_lookup(&cfg->routines, routine_hash);
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

routine_cfg_t *routine_cfg_new(uint routine_hash);
void routine_cfg_free(routine_cfg_t *cfg);
void routine_cfg_assign_opcode(routine_cfg_t *cfg, zend_uchar opcode, zend_uchar extended_value,
                               ushort line_number, uint index);
bool routine_cfg_has_opcode_edge(routine_cfg_t *cfg, uint from_index, uint to_index);
void routine_cfg_add_opcode_edge(routine_cfg_t *cfg, uint from_index, uint to_index,
                                 user_level_t user_level);

cfg_t *cfg_new();
void cfg_free(cfg_t *cfg);
void cfg_add_routine(cfg_t *cfg, routine_cfg_t *routine);
bool cfg_has_routine_edge(cfg_t *cfg, routine_cfg_t *from_routine, uint from_index,
                          routine_cfg_t *to_routine, uint to_index);
void cfg_add_routine_edge(cfg_t *cfg, routine_cfg_t *from_routine, uint from_index,
                          routine_cfg_t *to_routine, uint to_index,
                          user_level_t user_level);

#endif
