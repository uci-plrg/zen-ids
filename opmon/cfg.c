#include "cfg.h"

cfg_t *cfg_new(uint unit_hash, uint function_hash)
{
  cfg_t *cfg = malloc(sizeof(cfg_t));
  cfg->unit_hash = unit_hash;
  cfg->function_hash = function_hash;
  cfg->size = 0;
  return cfg;
}

void cfg_add_node(cfg_t *cfg, zend_uchar opcode)
{
  cfg->opcodes[cfg->size] = opcode;
  cfg->size++;
}

