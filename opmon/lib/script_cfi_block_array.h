#ifndef _SCRIPT_CFI_ARRAY_H_
#define _SCRIPT_CFI_ARRAY_H_ 1

#include "script_cfi_utils.h"

#define SCARRAY_GET(array, type, index) ((type *) scarray_get((array), index))

#define BLOCK_SIZE 0x40

typedef struct _scarray_data_t {
  uint item_count;
  void **blocks;
} scarray_data_t;

typedef struct _scarray_t {
  uint capacity;
  uint size;
  scarray_data_t data[BLOCK_SIZE];
} scarray_t;

typedef struct _scarray_iterator_t scarray_iterator_t;

void scarray_init(scarray_t *a);
void scarray_destroy(scarray_t *a);
void scarray_append(scarray_t *a, void *e);
void *scarray_get(scarray_t *a, uint index);

scarray_iterator_t *scarray_iterator_start(scarray_t *a);
void *scarray_iterator_next(scarray_iterator_t *iterator);
void scarray_iterator_end(scarray_iterator_t *iterator);

void scarray_unit_test();

#endif
