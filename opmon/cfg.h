#ifndef _CFG_H_
#define _CFG_H_ 1

#include "php.h"

typedef struct _cfg_node_t {
  zend_uchar opcode;
  uint index;
} cfg_node_t;

typedef struct _cfg_edge_t {
  uint from_index;
  uint to_index;
} cfg_edge_t;

typedef struct _cfg_t {
  uint unit_hash;
  uint function_hash;
  uint size;
  zend_uchar opcodes[256];
  cfg_edge_t edges[1]; // how many?
} cfg_t;

cfg_t *cfg_new(uint unit_hash, uint function_hash);
void cfg_add_node(cfg_t *cfg, zend_uchar opcode);

#endif
