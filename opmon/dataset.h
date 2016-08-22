#ifndef _DATASET_H_
#define _DATASET_H_ 1

#include "lib/script_cfi_utils.h"
#include "metadata_handler.h"

#define TARGET_ROUTINE_ID_MASK 0xffffffff
#define TARGET_INDEX_SHIFT 0x20
#define TARGET_INDEX_MASK 0xffffff
#define TARGET_USER_LEVEL_SHIFT 0x38
#define TARGET_USER_LEVEL_MASK 0x3f
#define TARGET_TYPE_MASK 0xc000000000000000
#define TARGET_TYPE_DATASET 0
#define TARGET_TYPE_SINGLETON 0x4000000000000000
#define TARGET_TYPE_EXPANDED 0x8000000000000000

#define MASK_TARGET_INDEX(to_index) ((to_index) & 0x3ffffff)
#define MASK_USER_LEVEL_SHIFT 0x1a
#define MASK_USER_LEVEL(to_index) ((to_index) >> MASK_USER_LEVEL_SHIFT)

typedef struct _dataset_call_target_t {
  uint routine_hash;
  uint index;
} dataset_call_target_t;

typedef struct _dataset_eval_target_t {
  uint eval_id;
  uint index;
} dataset_eval_target_t;

typedef struct _dataset_call_targets_t {
  uint target_count;
  dataset_call_target_t targets[1];
} dataset_call_targets_t;

typedef struct _dataset_eval_targets_t {
  uint target_count;
  dataset_eval_target_t targets[1];
} dataset_eval_targets_t;

typedef void * dataset_target_routines_t;

void *install_dataset(void *dataset);
uint dataset_get_eval_count(void *app);
dataset_routine_t *dataset_routine_lookup(application_t *app, uint routine_hash);
uint dataset_match_eval(control_flow_metadata_t *cfm, zend_op_array *op_array);

void dataset_routine_verify_compiled_edge(dataset_routine_t *dataset,
                                          uint from_index, uint to_index);
void dataset_routine_verify_opcode(dataset_routine_t *dataset, uint index,
                                   zend_uchar opcode);
bool dataset_verify_opcode_edge(dataset_routine_t *dataset, uint from_index,
                                uint to_index);
bool dataset_verify_routine_edge(application_t *app, dataset_routine_t *dataset, uint from_index,
                                 uint to_index, uint to_routine_hash, uint user_level);
dataset_target_routines_t *
dataset_lookup_target_routines(application_t *app, dataset_routine_t *routine, uint from_index);
uint dataset_get_call_target_count(application_t *app, dataset_routine_t *dataset, uint from_index);
uint dataset_routine_get_node_user_level(dataset_routine_t *dataset, uint index);
dataset_target_routines_t *
dataset_expand_target_routines(dataset_target_routines_t *original_targets,
                               uint routine_hash, uint to_index);

static inline bool dataset_verify_routine_target(dataset_target_routines_t *targets, uint target_id,
                                                 uint to_index, uint user_level, bool is_eval)
{
  uint i;
  uint64 metadata = p2int(targets);

  if ((metadata & TARGET_TYPE_MASK) == TARGET_TYPE_SINGLETON) {
    uint index = (metadata >> TARGET_INDEX_SHIFT) & TARGET_INDEX_MASK;
    uint min_trusted_user_level = (metadata >> TARGET_USER_LEVEL_SHIFT) & TARGET_USER_LEVEL_MASK;
    uint routine_id = metadata & TARGET_ROUTINE_ID_MASK;

    return (routine_id == target_id && index == to_index && min_trusted_user_level <= user_level);
  } else {
    if (is_eval) {
      dataset_eval_targets_t *eval_targets = (dataset_eval_targets_t *) (metadata & ~TARGET_TYPE_MASK);

      for (i = 0; i < eval_targets->target_count; i++) { // shouldn't we check the eval id?
        if (MASK_TARGET_INDEX(eval_targets->targets[i].index) == to_index &&
            MASK_USER_LEVEL(eval_targets->targets[i].index) <= user_level)
          return true;
      }
    } else {
      dataset_call_targets_t *call_targets = (dataset_call_targets_t *) (metadata & ~TARGET_TYPE_MASK);

      for (i = 0; i < call_targets->target_count; i++) {
        if (call_targets->targets[i].routine_hash == target_id &&
            MASK_TARGET_INDEX(call_targets->targets[i].index) == to_index &&
            MASK_USER_LEVEL(call_targets->targets[i].index) <= user_level)
          return true;
      }
      return false; // for debug stopping
    }
  }
  return false;
}

#endif
