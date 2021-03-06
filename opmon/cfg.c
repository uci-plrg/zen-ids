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
  routine_cfg_t *cfg = PROCESS_NEW(routine_cfg_t);
  cfg->routine_hash = routine_hash;
  scarray_init(&cfg->opcodes);
  scarray_init(&cfg->opcode_edges);
  return cfg;
}

routine_cfg_t *routine_cfg_new_empty(uint routine_hash)
{
  routine_cfg_t *cfg = PROCESS_NEW(routine_cfg_t);
  memset(cfg, 0, sizeof(routine_cfg_t));
  cfg->routine_hash = routine_hash;
  return cfg;
}

void routine_cfg_free(routine_cfg_t *cfg) // mem-todo
{
  scarray_destroy(&cfg->opcodes);
  scarray_destroy(&cfg->opcode_edges);
  PROCESS_FREE(cfg);
}

void routine_cfg_assign_opcode(routine_cfg_t *cfg, zend_uchar opcode, zend_ulong extended_value,
                               ushort line_number, uint index, user_level_t user_level)
{
  cfg_opcode_t *cfg_opcode;

  while (index >= cfg->opcodes.size) {
    cfg_opcode = PROCESS_NEW(cfg_opcode_t);
    memset(cfg_opcode, 0, sizeof(cfg_opcode_t));
    scarray_append(&cfg->opcodes, cfg_opcode);
  }

  cfg_opcode = routine_cfg_get_opcode(cfg, index);
  cfg_opcode->opcode = opcode;
  cfg_opcode->extended_value = extended_value;
  cfg_opcode->line_number = line_number;
  cfg_opcode->user_level = user_level;
}

cfg_opcode_edge_t *routine_cfg_lookup_opcode_edge(routine_cfg_t *routine,
                                                  uint from_index, uint to_index)
{
  uint i;

  for (i = 0; i < routine->opcode_edges.size; i++) {
    cfg_opcode_edge_t *edge = routine_cfg_get_opcode_edge(routine, i);
    if (edge->from_index == from_index && edge->to_index == to_index)
      return edge;
  }

  return NULL;
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
  cfg_opcode_edge_t *cfg_edge = PROCESS_NEW(cfg_opcode_edge_t); // todo: allocate in a block per op_array->last
  memset(cfg_edge, 0, sizeof(cfg_opcode_edge_t));
  scarray_append(&cfg->opcode_edges, cfg_edge);

  cfg_edge->from_index = from_index;
  cfg_edge->to_index = to_index;
  cfg_edge->user_level = user_level;
}

cfg_t *cfg_new()
{
  cfg_t *cfg = PROCESS_NEW(cfg_t);
  cfg->routines.hash_bits = 9;
  sctable_init(&cfg->routines);
  cfg->routine_edges.hash_bits = 10;
  sctable_init(&cfg->routine_edges);
  scarray_init(&cfg->evals);
  return cfg;
}

void cfg_free(cfg_t *cfg) // mem-todo
{
  if (cfg != NULL) {
    sctable_destroy(&cfg->routines);
    sctable_destroy(&cfg->routine_edges);
    scarray_destroy(&cfg->evals);
    PROCESS_FREE(cfg);
  }
}

void cfg_add_routine(cfg_t *cfg, routine_cfg_t *routine)
{
  sctable_add(&cfg->routines, routine->routine_hash, routine);
}

bool cfg_has_routine_edge(cfg_t *cfg, routine_cfg_t *from_routine, uint from_index,
                          routine_cfg_t *to_routine, uint to_index, uint user_level)
{
  uint64 key = HASH_ROUTINE_EDGE(from_routine, to_routine);
  cfg_routine_edge_entry_t *entry = sctable_lookup(&cfg->routine_edges, key);
  while (entry != NULL) {
    if (is_same_routine_cfg(entry->edge.from_routine, from_routine) &&
        entry->edge.from_index == from_index &&
        is_same_routine_cfg(entry->edge.to_routine, to_routine) &&
        entry->edge.to_index == to_index) {
      return (user_level >= entry->edge.user_level);
    }
    entry = entry->next;
  }
  return false;
}

// maybe change these two functions to cfg_add_or_update_routine_edge()?

void cfg_add_routine_edge(cfg_t *cfg, routine_cfg_t *from_routine, uint from_index,
                          routine_cfg_t *to_routine, uint to_index,
                          user_level_t user_level)
{
  uint64 key = HASH_ROUTINE_EDGE(from_routine, to_routine);
  cfg_routine_edge_entry_t *key_entry = sctable_lookup(&cfg->routine_edges, key);

  if (key_entry != NULL && is_same_routine_cfg(key_entry->edge.from_routine, from_routine) &&
      key_entry->edge.from_index == from_index &&
      is_same_routine_cfg(key_entry->edge.to_routine, to_routine) &&
      key_entry->edge.to_index == to_index) { /* on collision it might be down the chain--just add in that case */
    if (user_level < key_entry->edge.user_level)
      key_entry->edge.user_level = user_level;
  } else {
    cfg_routine_edge_entry_t *cfg_entry = PROCESS_NEW(cfg_routine_edge_entry_t);
    memset(cfg_entry, 0, sizeof(cfg_routine_edge_entry_t));
    cfg_entry->next = key_entry;
    sctable_add_or_replace(&cfg->routine_edges, key, cfg_entry);

    cfg_entry->edge.from_index = from_index;
    cfg_entry->edge.to_index = to_index;
    cfg_entry->edge.from_routine = from_routine;
    cfg_entry->edge.to_routine = to_routine;
    cfg_entry->edge.user_level = user_level;
  }
}

static bool is_same_eval_routine(routine_cfg_t *routine, zend_op_array *op_array)
{
  zend_op *op = op_array->opcodes, *end = op + op_array->last;
  cfg_opcode_t **node = (cfg_opcode_t **) routine->opcodes.data;

  if (routine->opcodes.size != op_array->last)
    return false;

  while (op < end) {
    if ((*node)->opcode != op->opcode)
      return false;
    op++;
    node++;
  }
  return true;
}

routine_cfg_t *cfg_get_matching_eval(cfg_t *cfg, zend_op_array *op_array)
{
  uint i;
  routine_cfg_t *routine;

  for (i = 0; i < cfg->evals.size; i++) {
    routine = (routine_cfg_t *) cfg->evals.data[i];
    if (is_same_eval_routine(routine, op_array))
      return routine;
  }
  return NULL;
}

void cfg_add_eval(cfg_t *cfg, routine_cfg_t *eval)
{
  scarray_append(&cfg->evals, eval);
}

const char *site_relative_path(application_t *app, zend_op_array *stack_frame)
{
  const char *filename = stack_frame->filename->val;
  if (strstr(filename, app->root) != NULL)
    return filename + strlen(app->root);
  else
    return filename;
}

