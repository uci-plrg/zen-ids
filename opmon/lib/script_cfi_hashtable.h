#ifndef _SCRIPT_CFI_HASHTABLE_H_
#define _SCRIPT_CFI_HASHTABLE_H_ 1

#include "php.h"

typedef struct _sctable_entry_t {
  uint key;
  void *payload;
  struct _sctable_entry_t *next;
} sctable_entry_t;


typedef struct _sctable_t {
  sctable_entry_t **data;
  uint  hash_bits;
  uint  hash_mask;
  uint  capacity;           /* = 2^bits */
  uint  entries;
  uint  load_factor_percent; /* \alpha = load_factor_percent/100 */
  uint  resize_threshold;    /*  = capacity * load_factor */
} sctable_t;

void
sctable_init(sctable_t *t);

void
sctable_add(sctable_t *t, uint key, void *value);

void
sctable_add_or_replace(sctable_t *t, uint key, void *value);

void
sctable_remove(sctable_t *t, uint key);

void
sctable_destroy(sctable_t *t);

#endif
