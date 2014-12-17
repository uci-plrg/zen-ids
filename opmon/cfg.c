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
