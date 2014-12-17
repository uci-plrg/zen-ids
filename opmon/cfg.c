#include "cfg.h"

routine_cfg_t *routine_cfg_new(uint unit_hash, uint function_hash)
{
  routine_cfg_t *cfg = malloc(sizeof(routine_cfg_t));
  cfg->unit_hash = unit_hash;
  cfg->function_hash = function_hash;
  cfg->opcode_count = cfg->edge_count = 0;
  return cfg;
}

void routine_cfg_add_node(routine_cfg_t *cfg, zend_uchar opcode)
{
  cfg->opcodes[cfg->opcode_count] = opcode;
  cfg->opcode_count++;
}

void routine_cfg_add_edge(routine_cfg_t *cfg, uint from_index, uint to_index)
{
  cfg->edges[cfg->edge_count].from_index = from_index;
  cfg->edges[cfg->edge_count].to_index = to_index;
  cfg->edge_count++;
}

cfg_t *cfg_new()
{
  cfg_t *cfg = malloc(sizeof(cfg_t));
  cfg->routine_count = 0;
  cfg->edge_count = 0;
  return cfg;
}

void cfg_add_routine(cfg_t *cfg, routine_cfg_t *routine)
{
  cfg->routines[cfg->routine_count] = routine;
  cfg->routine_count++;
}

void cfg_add_routine_edge(cfg_t *cfg, cfg_node_t from_node, 
                          routine_cfg_t *from_routine, routine_cfg_t *to_routine)
{
  cfg->edges[cfg->edge_count].from_node = from_node;
  cfg->edges[cfg->edge_count].from_routine = from_routine;
  cfg->edges[cfg->edge_count].to_routine = to_routine;
  cfg->edge_count++;
}
