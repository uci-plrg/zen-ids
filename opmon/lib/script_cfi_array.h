#ifndef _SCRIPT_CFI_ARRAY_H_
#define _SCRIPT_CFI_ARRAY_H_ 1

#include "script_cfi_utils.h"

typedef struct _scarray_t scarray_t;

void scarray_init(scarray_t *a);
void scarray_destroy(scarray_t *a);
void scarray_append(scarray_t *a, void *e);
void *scarray_get(scarray_t *a, uint index);

void scarray_unit_test();

#endif
