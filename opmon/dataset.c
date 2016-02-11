#include "php.h"

#include "lib/script_cfi_utils.h"
#include "cfg.h"
#include "dataset.h"

#pragma pack(push, 4)

// todo: edges may also need a type { call, exception }

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

typedef enum _dataset_node_type_t {
  DATASET_NODE_TYPE_NORMAL,
  DATASET_NODE_TYPE_BRANCH,
  //DATASET_NODE_TYPE_DIRECT_BRANCH,
  //DATASET_NODE_TYPE_INDIRECT_BRANCH,
  DATASET_NODE_TYPE_CALL,
  DATASET_NODE_TYPE_EVAL
} dataset_node_type_t;

typedef struct _dataset_node_t {
  zend_uchar opcode;
  zend_uchar type;
  zend_uchar pad[2];
  union {
    uint zero; // [normal node]
    uint target_index; // [branch node]
    uint call_targets; // dataset_call_targets_t * [call node] (relative to dataset top)
    uint eval_targets; // dataset_eval_targets_t * [eval node] (relative to dataset top)
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
#define MASK_TARGET_INDEX(to_index) ((to_index) & 0x3ffffff)
#define MASK_USER_LEVEL(to_index) ((to_index) >> 0x1b)

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
  dataset_app_t *dataset_app = PROCESS_ALLOC(dataset_app_t);
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

static bool match_eval_routines(dataset_routine_t *dataset, routine_cfg_t *routine)
{
  uint i;

  if (dataset->node_count != routine->opcodes.size)
    return false;
  for (i = 0; i < dataset->node_count; i++) {
    if (dataset->nodes[i].opcode != routine_cfg_get_opcode(routine, i)->opcode)
      return false;
  }
  return true;
}

void dataset_match_eval(control_flow_metadata_t *cfm)
{
  uint i;
  dataset_routine_t *routine;
  dataset_app_t *dataset = (dataset_app_t *) cfm->app->dataset;

  if (dataset == NULL)
    return;

  for (i = 0; i < dataset->eval_list->count; i++) {
    routine = RESOLVE_PTR(dataset, dataset->eval_list->list[i], dataset_routine_t);
    if (match_eval_routines(routine, cfm->cfg)) {
      MON("Matched eval %d to dataset eval %d\n", get_eval_id(cfm->cfg->routine_hash), i);

      cfm->dataset = routine;
      cfm->cfg->routine_hash = hash_eval(i);
      return;
    }
  }

  MON("Failed to match eval %d\n", cfm->cfg->routine_hash);
}

void dataset_routine_verify_compiled_edge(dataset_routine_t *dataset,
                                          uint from_index, uint to_index)
{
  dataset_node_t *node = &dataset->nodes[from_index];
  if (to_index == (from_index + 1) || node->target_index == to_index ||
      is_fall_through(node->opcode, from_index, to_index)) {
    MON("Verified compiled edge from %d to %d\n", from_index, to_index);
  } else {
    MON("Opcode edge mismatch at index %d: expected target %d but found target %d\n",
          from_index, node->target_index, to_index);
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

    if (node->type == DATASET_NODE_TYPE_CALL) {
      dataset_call_targets_t *targets = RESOLVE_PTR(dataset, node->call_targets,
                                                    dataset_call_targets_t);
      for (i = 0; i < targets->target_count; i++) {
        if (targets->targets[i].routine_hash == to_routine_hash &&
            MASK_TARGET_INDEX(targets->targets[i].index) == to_index &&
            MASK_USER_LEVEL(targets->targets[i].index) <= user_level)
          return true;
      }
      return false; // for debug stopping
    } else if (node->type == DATASET_NODE_TYPE_EVAL) {
      dataset_eval_targets_t *targets = RESOLVE_PTR(dataset, node->eval_targets,
                                                    dataset_eval_targets_t);
      for (i = 0; i < targets->target_count; i++) {
        if (MASK_TARGET_INDEX(targets->targets[i].index) == to_index &&
            MASK_USER_LEVEL(targets->targets[i].index) <= user_level)
          return true;
      }
      return false; // for debug stopping
    }
  }
  return false;
}

uint dataset_get_call_target_count(application_t *app, dataset_routine_t *routine, uint from_index)
{
  if (app->dataset != NULL) {
    dataset_node_t *node = &routine->nodes[from_index];
    dataset_app_t *dataset = (dataset_app_t *) app->dataset;
    if (node->type == DATASET_NODE_TYPE_CALL) {
      dataset_call_targets_t *targets = RESOLVE_PTR(dataset, node->call_targets,
                                                    dataset_call_targets_t);
      return targets->target_count;
    }
  }
  return 0;
}
