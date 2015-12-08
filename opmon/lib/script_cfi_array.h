#ifndef _SCRIPT_CFI_ARRAY_H_
#define _SCRIPT_CFI_ARRAY_H_ 1

#include "script_cfi_utils.h"

#define SCARRAY_ERROR_INVALID_ELEMENT (~0U)

#define SCARRAY_GET(array, type, index) ((type *) scarray_get((array), index))

typedef struct _scarray_t {
  uint capacity;
  uint size;
  void **data;
} scarray_t;

typedef struct _scarray_iterator_t scarray_iterator_t;

void scarray_init(scarray_t *a);
void scarray_destroy(scarray_t *a);
void scarray_append(scarray_t *a, void *e);
void *scarray_get(scarray_t *a, uint index);
void* scarray_remove(scarray_t *a, uint index);

scarray_iterator_t *scarray_iterator_start(scarray_t *a);
scarray_iterator_t *scarray_iterator_start_at(scarray_t *a, uint start_index);
void *scarray_iterator_next(scarray_iterator_t *iterator);
uint scarray_iterator_index(scarray_t *a, scarray_iterator_t *i);
void scarray_iterator_end(scarray_iterator_t *iterator);

void scarray_unit_test();

#endif
