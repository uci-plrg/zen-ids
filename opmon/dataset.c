#include "php.h"

#include "lib/script_cfi_utils.h"
#include "dataset.h"

typedef struct _dataset_call_target_t {
  uint unit_hash;
  uint routine_hash;
} dataset_call_target_t;

typedef struct _dataset_call_targets_t {
  uint target_count;
  dataset_call_target_t *targets;
} dataset_call_targets_t;

typedef struct _dataset_node_t {
  zend_uchar pad[3];
  zend_uchar opcode;
  union {
    uint zero; // normal node
    uint target_index; // branch node
    dataset_call_targets_t *call_targets; // call node (relative to dataset top)
    uint something; // eval node
  };
} dataset_node_t;

struct _dataset_routine_t {
  uint unit_hash;
  uint routine_hash;
  uint node_count;
  dataset_node_t *nodes;
};

typedef struct _dataset_chain_t {
  dataset_routine_t *routine; // (relative to dataset top)
} dataset_chain_t;

typedef struct _dataset_hashtable_t {
  uint mask;
  dataset_chain_t **table; // (relative to dataset top)
} dataset_hashtable_t;

static uint_ptr_t dataset_mapping;
static dataset_hashtable_t *hashtable;

#define RESOLVE_PTR(ptr, type) ((type *)(dataset_mapping + (uint_ptr_t)(ptr)))

void install_dataset(void *dataset)
{
  dataset_mapping = (uint_ptr_t) dataset;
  hashtable = (dataset_hashtable_t *) dataset_mapping;
}

dataset_routine_t *lookup_routine(uint unit_hash, uint routine_hash)
{
  uint index = (unit_hash ^ routine_hash) & hashtable->mask;
  dataset_routine_t *routine;
  dataset_chain_t *chain = RESOLVE_PTR(hashtable->table[index], dataset_chain_t);
  while (chain != NULL) {
    routine = RESOLVE_PTR(chain->routine, dataset_routine_t);
    if (routine->unit_hash == unit_hash && routine->routine_hash == routine_hash)
      return routine;
    chain++;
  }
  return NULL;
}
