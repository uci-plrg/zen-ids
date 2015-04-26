#ifndef _SCRIPT_CFI_HASHTABLE_H_
#define _SCRIPT_CFI_HASHTABLE_H_ 1

#include "php.h"
#include "script_cfi_utils.h"

#define KEY_TYPE uint64

typedef struct _sctable_entry_t {
  KEY_TYPE key;
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

/* caller must have previously set `t->hash_bits` */
void
sctable_init(sctable_t *t);

void *
sctable_lookup(sctable_t *t, KEY_TYPE key);

void
sctable_add(sctable_t *t, KEY_TYPE key, void *value);

void
sctable_add_or_replace(sctable_t *t, KEY_TYPE key, void *value);

void
sctable_remove(sctable_t *t, KEY_TYPE key);

void
sctable_clear(sctable_t *t);

void
sctable_destroy(sctable_t *t);

#undef KEY_TYPE

#endif
