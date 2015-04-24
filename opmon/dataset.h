#ifndef _DATASET_H_
#define _DATASET_H_ 1

#include "lib/script_cfi_utils.h"
#include "metadata_handler.h"

void *install_dataset(void *dataset);
uint dataset_get_eval_count(void *app);
dataset_routine_t *dataset_routine_lookup(application_t *app, uint routine_hash);
void dataset_match_eval(control_flow_metadata_t *cfm);

void dataset_routine_verify_compiled_edge(dataset_routine_t *dataset,
                                          uint from_index, uint to_index);
void dataset_routine_verify_opcode(dataset_routine_t *dataset, uint index,
                                   zend_uchar opcode);
bool dataset_verify_opcode_edge(dataset_routine_t *dataset, uint from_index,
                                uint to_index);
bool dataset_verify_routine_edge(application_t *app, dataset_routine_t *dataset, uint from_index,
                                 uint to_index, uint to_routine_hash, uint user_level);
uint dataset_get_call_target_count(application_t *app, dataset_routine_t *dataset, uint from_index);

#endif
