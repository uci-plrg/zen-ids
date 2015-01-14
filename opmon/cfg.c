#include "cfg.h"

/*
typedef struct _routine_cfg_t {
  uint unit_hash;
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

routine_cfg_t *routine_cfg_new(uint unit_hash, uint routine_hash)
{
  routine_cfg_t *cfg = malloc(sizeof(routine_cfg_t));
  cfg->unit_hash = unit_hash;
  cfg->routine_hash = routine_hash;
  scarray_init(&cfg->opcodes);
  scarray_init(&cfg->opcode_edges);
  scarray_init(&cfg->routine_edges);
  return cfg;
}

void routine_cfg_assign_opcode(routine_cfg_t *cfg, zend_uchar opcode, 
                               zend_uchar extended_value, uint index)
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

void routine_cfg_add_opcode_edge(routine_cfg_t *cfg, uint from_index, uint to_index)
{
  cfg_opcode_edge_t *cfg_edge = malloc(sizeof(cfg_opcode_edge_t));
  memset(cfg_edge, 0, sizeof(cfg_opcode_edge_t));
  scarray_append(&cfg->opcode_edges, cfg_edge);
  
  cfg_edge->from_index = from_index;
  cfg_edge->to_index = to_index;
}

cfg_t *cfg_new()
{
  cfg_t *cfg = malloc(sizeof(cfg_t));
  scarray_init(&cfg->routines);
  return cfg;
}

void cfg_add_routine(cfg_t *cfg, routine_cfg_t *routine)
{
  scarray_append(&cfg->routines, routine);
}

bool cfg_has_routine_edge(routine_cfg_t *from_routine, uint from_index, 
                          routine_cfg_t *to_routine, uint to_index)
{
  uint i;
  cfg_routine_edge_t *existing_edge;
  for (i = 0; i < from_routine->routine_edges.size; i++) {
    existing_edge = routine_cfg_get_routine_edge(from_routine, i);
    if (existing_edge->from_index == from_index &&
        is_same_routine_cfg(existing_edge->to_routine, to_routine) &&
        existing_edge->to_index == to_index) {
      return true;
    }
  }
  return false;
}

void cfg_add_routine_edge(routine_cfg_t *from_routine, uint from_index, 
                          routine_cfg_t *to_routine, uint to_index)
{
  cfg_routine_edge_t *cfg_edge = malloc(sizeof(cfg_routine_edge_t));
  memset(cfg_edge, 0, sizeof(cfg_routine_edge_t));
  scarray_append(&from_routine->routine_edges, cfg_edge);
  
  cfg_edge->from_index = from_index;
  cfg_edge->to_index = to_index;
  cfg_edge->from_routine = from_routine;
  cfg_edge->to_routine = to_routine;
}
