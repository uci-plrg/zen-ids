#include "php.h"

#include "lib/script_cfi_utils.h"
#include "cfg.h"
#include "dataset.h"

#pragma pack(push, 4)

// todo: edges may also need a type { call, exception }

typedef enum _dataset_node_type_flags_t {
  DATASET_NODE_TYPE_BRANCH = 0x1,
  DATASET_NODE_TYPE_CALL   = 0x2,
  DATASET_NODE_TYPE_EVAL   = 0x4,
} dataset_node_type_flags_t;

typedef struct _dataset_node_t {
  zend_uchar opcode;
  zend_uchar type_flags; /* of dataset_node_type_flags_t */
  ushort line_number;
  union {
    uint user_level;   // normal node: [user level | zero]
    uint target_index; // branch node: [user level | branch target index]
  };
  union {
    uint call_targets; // call node:   dataset_call_targets_t * (relative to dataset top)
    uint eval_targets; // eval node:   dataset_eval_targets_t * (relative to dataset top)
  };
} dataset_node_t;

struct _dataset_routine_t {
  uint routine_hash;
  uint node_count;
  dataset_node_t nodes[1];
};

typedef struct _dataset_chain_t {
  uint routine; // dataset_routine_t * (relative to dataset top)
} dataset_chain_t;

typedef struct _dataset_hashtable_t {
  uint mask;
  uint table[1]; // dataset_chain_t * (relative to dataset top)
} dataset_hashtable_t;

typedef struct _dataset_eval_list_t {
  uint count;
  uint list[1];
} dataset_eval_list_t;

typedef struct _dataset_app_t {
  uint_ptr_t dataset_mapping;
  dataset_hashtable_t *hashtable;
  dataset_eval_list_t *eval_list;
} dataset_app_t;

#pragma pack(pop)

#define RESOLVE_PTR(app, ptr, type) ((type *)((app)->dataset_mapping + ((uint_ptr_t)(ptr) * 4)))
#define IS_CHAIN_TERMINUS(chain) (*(uint *)chain == 0)
#define SET_USER_LEVEL(node, user_level) ((node)->user_level

static bool is_fall_through(zend_uchar opcode, uint from_index, uint to_index) {
  switch (opcode) {
    case ZEND_NEW:
    case ZEND_ASSIGN_DIM:
      return to_index == (from_index + 2);
    default:
      return false;
  }
}

void *install_dataset(void *dataset)
{
  dataset_app_t *dataset_app = PROCESS_NEW(dataset_app_t);
  dataset_app->dataset_mapping = (uint_ptr_t) dataset;
  dataset_app->hashtable = RESOLVE_PTR(dataset_app, *(uint *) dataset_app->dataset_mapping,
                                       dataset_hashtable_t);
  dataset_app->eval_list = (dataset_eval_list_t *)(dataset_app->hashtable->table +
                                                   (dataset_app->hashtable->mask + 1));
  return dataset_app;
}

uint dataset_get_eval_count(void *dataset)
{
  return ((dataset_app_t *) dataset)->eval_list->count;
}

dataset_routine_t *dataset_routine_lookup(application_t *app, uint routine_hash)
{
  if (app->dataset != NULL) {
    dataset_app_t *dataset_app = (dataset_app_t *) app->dataset;
    if (!is_eval_routine(routine_hash)) {
      uint index = routine_hash & dataset_app->hashtable->mask;
      dataset_routine_t *routine;
      dataset_chain_t *chain;

      if (dataset_app->hashtable->table[index] == 0)
        return NULL;

      chain = RESOLVE_PTR(dataset_app, dataset_app->hashtable->table[index], dataset_chain_t);
      while (!IS_CHAIN_TERMINUS(chain)) {
        routine = RESOLVE_PTR(dataset_app, chain->routine, dataset_routine_t);
        if (routine->routine_hash == routine_hash)
          return routine;
        chain++;
      }
    }
  }
  return NULL;
}

static bool match_eval_routine(dataset_routine_t *dataset, zend_op_array *op_array)
{
  zend_op *op = op_array->opcodes, *end = op + op_array->last;
  dataset_node_t *node = dataset->nodes;

  if (dataset->node_count != op_array->last)
    return false;

  while (op < end) {
    if (node->opcode != op->opcode)
      return false;
    op++;
    node++;
  }
  return true;
}

uint dataset_match_eval(control_flow_metadata_t *cfm, zend_op_array *op_array)
{
  uint i;
  dataset_routine_t *routine;
  dataset_app_t *dataset = (dataset_app_t *) cfm->app->dataset;

  if (dataset == NULL)
    return 0;

  for (i = 0; i < dataset->eval_list->count; i++) {
    routine = RESOLVE_PTR(dataset, dataset->eval_list->list[i], dataset_routine_t);
    if (match_eval_routine(routine, op_array)) {
      MON("Matched eval to dataset eval %d\n", get_eval_id(routine->routine_hash));

      cfm->dataset = routine;
      return routine->routine_hash;
    }
  }

  MON("Failed to match eval %d\n", cfm->cfg->routine_hash);
  return 0;
}

void dataset_routine_verify_compiled_edge(dataset_routine_t *dataset,
                                          uint from_index, uint to_index)
{
  dataset_node_t *node = &dataset->nodes[from_index];
  if (to_index == (from_index + 1) || MASK_TARGET_INDEX(node->target_index) == to_index ||
      is_fall_through(node->opcode, from_index, to_index)) {
    MON("Verified compiled edge from %d to %d\n", from_index, to_index);
  } else {
    MON("Opcode edge mismatch at index %d: expected target %d but found target %d\n",
          from_index, MASK_TARGET_INDEX(node->target_index), to_index);
  }
}

void dataset_routine_verify_opcode(dataset_routine_t *dataset, uint index,
                                   zend_uchar opcode)
{
  dataset_node_t *node = &dataset->nodes[index];
  if (node->opcode == opcode) {
    MON("Verified opcode %d at index %d\n", opcode, index);
  } else {
    MON("Opcode mismatch at index %d: expected %d but found %d\n", index,
          node->opcode, opcode);
  }
}

bool dataset_verify_opcode_edge(dataset_routine_t *dataset, uint from_index,
                                uint to_index)
{
  return true;
}

bool dataset_verify_routine_edge(application_t *app, dataset_routine_t *routine, uint from_index,
                                 uint to_index, uint to_routine_hash, uint user_level)
{
  if (app->dataset != NULL) {
    uint i;
    dataset_node_t *node = &routine->nodes[from_index];
    dataset_app_t *dataset = (dataset_app_t *) app->dataset;

    if (TEST(DATASET_NODE_TYPE_CALL, node->type_flags)) {
      dataset_call_targets_t *targets = RESOLVE_PTR(dataset, node->call_targets,
                                                    dataset_call_targets_t);
      for (i = 0; i < targets->target_count; i++) {
        if (targets->targets[i].routine_hash == to_routine_hash &&
            MASK_TARGET_INDEX(targets->targets[i].index) == to_index &&
            MASK_USER_LEVEL(targets->targets[i].index) <= user_level)
          return true;
      }
      return false; // for debug stopping
    } else if (TEST(DATASET_NODE_TYPE_EVAL, node->type_flags)) {
      dataset_eval_targets_t *targets = RESOLVE_PTR(dataset, node->eval_targets,
                                                    dataset_eval_targets_t);
      for (i = 0; i < targets->target_count; i++) { // shouldn't we check the eval id?
        if (targets->targets[i].index == to_index &&
            MASK_USER_LEVEL(targets->targets[i].index) <= user_level)
          return true;
      }
      return false; // for debug stopping
    }
  }
  return false;
}

static dataset_target_routines_t *pack_singleton_call_target(uint routine_hash, uint index)
{
  uint64 metadata = routine_hash;
  uint64 metadata_user_level = MASK_USER_LEVEL(index);
  uint64 metadata_index = MASK_TARGET_INDEX(index & TARGET_INDEX_MASK);

  metadata |= TARGET_TYPE_SINGLETON;
  metadata |= (metadata_index << TARGET_INDEX_SHIFT);
  metadata |= (metadata_user_level << TARGET_USER_LEVEL_SHIFT);
  return (dataset_target_routines_t *) (void *) metadata;
}

static dataset_target_routines_t *pack_singleton_eval_target(dataset_eval_target_t *target)
{
  uint64 metadata = target->eval_id;
  uint64 user_level = MASK_USER_LEVEL(target->index);
  uint64 index = MASK_TARGET_INDEX(target->index & TARGET_INDEX_MASK);

  metadata |= TARGET_TYPE_SINGLETON;
  metadata |= (index << TARGET_INDEX_SHIFT);
  metadata |= (user_level << TARGET_USER_LEVEL_SHIFT);
  return (dataset_target_routines_t *) (void *) metadata;
}

dataset_target_routines_t *
dataset_lookup_target_routines(application_t *app, dataset_routine_t *routine, uint from_index)
{
  if (app->dataset != NULL) {
    dataset_node_t *node = &routine->nodes[from_index];
    dataset_app_t *dataset = (dataset_app_t *) app->dataset;

    if (TEST(DATASET_NODE_TYPE_CALL, node->type_flags)) {
      dataset_call_targets_t *targets;
      targets = (dataset_call_targets_t *) RESOLVE_PTR(dataset, node->call_targets,
                                                       dataset_call_targets_t);
      if (targets->target_count == 1)
        return pack_singleton_call_target(targets->targets[0].routine_hash, targets->targets[0].index);
      else if (targets->target_count > 0)
        return (dataset_target_routines_t *) targets;
    } else if (TEST(DATASET_NODE_TYPE_EVAL, node->type_flags)) {
      dataset_eval_targets_t *targets;
      targets = (dataset_eval_targets_t *) RESOLVE_PTR(dataset, node->eval_targets,
                                                       dataset_eval_targets_t);
      if (targets->target_count == 1)
        return pack_singleton_eval_target(&targets->targets[0]);
      else if (targets->target_count > 0)
        return (dataset_target_routines_t *) targets;
    }
  }
  return NULL;
}

/* `original_targets` may be NULL, or a singleton, or an mmapped or expanded dataset_(eval|call)_targets */
dataset_target_routines_t *
dataset_expand_target_routines(dataset_target_routines_t *original_targets,
                               uint routine_hash, uint to_index)
{
  uint i, original_target_count;
  bool free_original_targets = false;
  uint64 metadata = p2int(original_targets);
  dataset_call_targets_t *original_call_targets, *expanded_routines;

  if (original_targets == NULL || ((metadata & TARGET_TYPE_MASK) != TARGET_TYPE_SINGLETON &&
                                   ((dataset_call_targets_t *) (metadata & ~TARGET_TYPE_MASK))->target_count == 0))
    return pack_singleton_call_target(routine_hash, to_index);

  switch (metadata & TARGET_TYPE_MASK) {
    case TARGET_TYPE_EXPANDED:
      free_original_targets = true;
    case TARGET_TYPE_DATASET: /* FT */
      original_call_targets = (dataset_call_targets_t *) (metadata & ~TARGET_TYPE_MASK);
      original_target_count = original_call_targets->target_count;
      break;
    case TARGET_TYPE_SINGLETON:
      original_target_count = 1;
      break;
  }

  expanded_routines = PROCESS_ALLOC(sizeof(dataset_call_targets_t) + (sizeof(dataset_call_target_t) * original_target_count));
  expanded_routines->target_count = original_target_count + 1;
  expanded_routines->targets[0].routine_hash = routine_hash;
  expanded_routines->targets[0].index = to_index;

  switch (metadata & TARGET_TYPE_MASK) {
    case TARGET_TYPE_EXPANDED:
    case TARGET_TYPE_DATASET:
      for (i = 0; i < original_call_targets->target_count; i++)
        expanded_routines->targets[i+1] = original_call_targets->targets[i];
      break;
    case TARGET_TYPE_SINGLETON: {
      uint singleton_index = (metadata >> TARGET_INDEX_SHIFT) & TARGET_INDEX_MASK;
      uint singleton_user_level = (metadata >> TARGET_USER_LEVEL_SHIFT) & TARGET_USER_LEVEL_MASK;
      uint singleton_routine_hash = metadata & TARGET_ROUTINE_ID_MASK;

      expanded_routines->targets[1].routine_hash = singleton_routine_hash;
      expanded_routines->targets[1].index = singleton_index | (singleton_user_level << MASK_USER_LEVEL_SHIFT);
    } break;
  }

  metadata = p2int(expanded_routines);
  expanded_routines = (dataset_call_targets_t *) (metadata | TARGET_TYPE_EXPANDED);

  if (free_original_targets)
    PROCESS_FREE(original_call_targets);

  return (dataset_target_routines_t *) expanded_routines;
}

uint dataset_get_call_target_count(application_t *app, dataset_routine_t *routine, uint from_index)
{
  if (app->dataset != NULL) {
    dataset_node_t *node = &routine->nodes[from_index];
    dataset_app_t *dataset = (dataset_app_t *) app->dataset;
    if (TEST(DATASET_NODE_TYPE_CALL, node->type_flags)) {
      dataset_call_targets_t *targets = RESOLVE_PTR(dataset, node->call_targets,
                                                    dataset_call_targets_t);
      return targets->target_count;
    }
  }
  return 0;
}

uint dataset_routine_get_node_user_level(dataset_routine_t *dataset, uint index)
{
  dataset_node_t *node = &dataset->nodes[index];
  return MASK_USER_LEVEL(node->user_level);
}
