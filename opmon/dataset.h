#ifndef _DATASET_H_
#define _DATASET_H_ 1

#include "lib/script_cfi_utils.h"

typedef struct _dataset_routine_t dataset_routine_t;

void install_dataset(void *dataset_mapping);
dataset_routine_t *dataset_routine_lookup(uint unit_hash, uint routine_hash);
void dataset_routine_verify_compiled_edge(dataset_routine_t *dataset, 
                                          uint from_index, uint to_index);
void dataset_routine_verify_opcode(dataset_routine_t *dataset, uint index, 
                                   zend_uchar opcode);
bool dataset_verify_routine_edge(dataset_routine_t *dataset, uint from_index, 
                                 uint to_unit_hash, uint to_routine_hash);
#endif
