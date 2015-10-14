#include "cfg.h"

/*
typedef struct _routine_cfg_t {
  uint routine_hash;
  uint opcode_count;
  uint edge_count;
  scarray_t opcodes; // cfg_opcode_t *
  scarray_t edges;   // cfg_opcode_edge_t *
} routine_cfg_t;

typedef struct _cfg_t {
  uint routine_count;
  uint edge_count;
  scarray_t routines; // routine_cfg_t *
  scarray_t edges;    // routine_edge_t *
} cfg_t;
*/

#define HASH_ROUTINE_EDGE(from, to) \
  ((((uint64)(from)->routine_hash) << 0x20) | ((uint64)(to)->routine_hash))

typedef struct _cfg_routine_edge_entry_t {
  cfg_routine_edge_t edge;
  struct _cfg_routine_edge_entry_t *next;
} cfg_routine_edge_entry_t;

routine_cfg_t *routine_cfg_new(uint routine_hash)
{
  routine_cfg_t *cfg = malloc(sizeof(routine_cfg_t));
  cfg->routine_hash = routine_hash;
  scarray_init(&cfg->opcodes);
  scarray_init(&cfg->opcode_edges);
  return cfg;
}

void routine_cfg_free(routine_cfg_t *cfg) // mem-todo
{
  scarray_destroy(&cfg->opcodes);
  scarray_destroy(&cfg->opcode_edges);
  free(cfg);
}

void routine_cfg_assign_opcode(routine_cfg_t *cfg, zend_uchar opcode, zend_ulong extended_value,
                               ushort line_number, uint index)
{
  cfg_opcode_t *cfg_opcode;

  while (index >= cfg->opcodes.size) {
    cfg_opcode = malloc(sizeof(cfg_opcode_t));
    memset(cfg_opcode, 0, sizeof(cfg_opcode_t));
    scarray_append(&cfg->opcodes, cfg_opcode);
  }

  cfg_opcode = routine_cfg_get_opcode(cfg, index);
  cfg_opcode->opcode = opcode;
  cfg_opcode->extended_value = extended_value;
  cfg_opcode->line_number = line_number;
}

bool routine_cfg_has_opcode_edge(routine_cfg_t *cfg, uint from_index, uint to_index)
{
  uint i;
  cfg_opcode_edge_t *existing_edge;
  for (i = 0; i < cfg->opcode_edges.size; i++) {
    existing_edge = routine_cfg_get_opcode_edge(cfg, i);
    if (existing_edge->from_index == from_index && existing_edge->to_index == to_index)
      return true;
  }
  return false;
}

void routine_cfg_add_opcode_edge(routine_cfg_t *cfg, uint from_index, uint to_index,
                                 user_level_t user_level)
{
  cfg_opcode_edge_t *cfg_edge = malloc(sizeof(cfg_opcode_edge_t));
  memset(cfg_edge, 0, sizeof(cfg_opcode_edge_t));
  scarray_append(&cfg->opcode_edges, cfg_edge);

  cfg_edge->from_index = from_index;
  cfg_edge->to_index = to_index;
  cfg_edge->user_level = user_level;

  if (cfg->routine_hash == 0xfc6651c2) {
    SPOT("Compiling opcode edge %d -> %d at level %d in (0x%x)\n",
         from_index, to_index, user_level, cfg->routine_hash);
  }
}

cfg_t *cfg_new()
{
  cfg_t *cfg = malloc(sizeof(cfg_t));
  cfg->routines.hash_bits = 9;
  sctable_init(&cfg->routines);
  cfg->routine_edges.hash_bits = 10;
  sctable_init(&cfg->routine_edges);
  return cfg;
}

void cfg_free(cfg_t *cfg) // mem-todo
{
  if (cfg != NULL) {
    sctable_destroy(&cfg->routines);
    sctable_destroy(&cfg->routine_edges);
    free(cfg);
  }
}

void cfg_add_routine(cfg_t *cfg, routine_cfg_t *routine)
{
  sctable_add(&cfg->routines, routine->routine_hash, routine);
}

bool cfg_has_routine_edge(cfg_t *cfg, routine_cfg_t *from_routine, uint from_index,
                          routine_cfg_t *to_routine, uint to_index)
{
  uint64 key = HASH_ROUTINE_EDGE(from_routine, to_routine);
  cfg_routine_edge_entry_t *entry = sctable_lookup(&cfg->routine_edges, key);
  while (entry != NULL) {
    if (is_same_routine_cfg(entry->edge.from_routine, from_routine) &&
        entry->edge.from_index == from_index &&
        is_same_routine_cfg(entry->edge.to_routine, to_routine) &&
        entry->edge.to_index == to_index) {
      return true;
    }
    entry = entry->next;
  }
  return false;
}

void cfg_add_routine_edge(cfg_t *cfg, routine_cfg_t *from_routine, uint from_index,
                          routine_cfg_t *to_routine, uint to_index,
                          user_level_t user_level)
{
  uint64 key = HASH_ROUTINE_EDGE(from_routine, to_routine);
  cfg_routine_edge_entry_t *key_entry = sctable_lookup(&cfg->routine_edges, key);
  cfg_routine_edge_entry_t *cfg_entry = malloc(sizeof(cfg_routine_edge_entry_t));
  memset(cfg_entry, 0, sizeof(cfg_routine_edge_entry_t));
  cfg_entry->next = key_entry;
  sctable_add_or_replace(&cfg->routine_edges, key, cfg_entry);

  cfg_entry->edge.from_index = from_index;
  cfg_entry->edge.to_index = to_index;
  cfg_entry->edge.from_routine = from_routine;
  cfg_entry->edge.to_routine = to_routine;
  cfg_entry->edge.user_level = user_level;
}
