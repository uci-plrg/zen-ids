#ifndef _DATASET_H_
#define _DATASET_H_ 1

typedef struct _dataset_routine_t dataset_routine_t;

void install_dataset(void *dataset_mapping);
dataset_routine_t *lookup_routine(uint unit_hash, uint routine_hash);

#endif
