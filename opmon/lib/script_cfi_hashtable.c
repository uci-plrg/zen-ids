#include "script_cfi_hashtable.h"

#define HASH_TAG_BITS (sizeof(uint))
#define UINT_0 ((uint) 0U)

#define HASH_FUNC(key, table) key & table->hash_mask
#define HASH_MASK(num_bits) ((~UINT_0) >> (HASH_TAG_BITS-(num_bits)))
#define HASHTABLE_SIZE(num_bits) (1U << (num_bits))

#define TABLE_ALLOC(size) malloc(size)
#define TABLE_FREE(ptr) free(ptr)

static void
sctable_insert(sctable_t *t, sctable_entry_t *e)
{
    uint hindex = HASH_FUNC(e->key, t);
    e->next = t->data[hindex];
    t->data[hindex] = e;
    t->entries++;
}

static void
sctable_resize(sctable_t *t)
{
    sctable_entry_t *e, *next_e;
    sctable_entry_t **old_data = t->data;
    uint old_capacity = t->capacity;
    uint i;

    t->hash_bits++;
    sctable_init(t);

    for (i = 0; i < old_capacity; i++) {
        for (e = old_data[i]; e != NULL; e = next_e) {
            next_e = e->next;
            sctable_insert(t, e);
        }
    }

    TABLE_FREE(old_data); // todo! fix segfault
}

static sctable_entry_t *
sctable_lookup_entry(sctable_t *t, uint key)
{
    sctable_entry_t *e;
    uint hindex = HASH_FUNC(key, t);
    for (e = t->data[hindex]; e; e = e->next) {
        if (e->key == key)
            return e;
    }
    return NULL;
}

void
sctable_init(sctable_t *t)
{
    t->hash_mask = HASH_MASK(t->hash_bits);
    t->capacity = HASHTABLE_SIZE(t->hash_bits);
    t->entries = 0;
    t->load_factor_percent = 70;
    t->resize_threshold =
        t->capacity * t->load_factor_percent / 100;
    t->data = (sctable_entry_t **)
        TABLE_ALLOC(t->capacity * sizeof(sctable_entry_t *));
    memset(t->data, 0, t->capacity * sizeof(sctable_entry_t *));
}

void *
sctable_lookup(sctable_t *t, uint key)
{
    sctable_entry_t *e;
    uint hindex = HASH_FUNC(key, t);
    for (e = t->data[hindex]; e; e = e->next) {
        if (e->key == key)
            return e->payload;
    }
    return NULL;
}

void
sctable_add(sctable_t *t, uint key, void *value)
{
  sctable_entry_t *e = TABLE_ALLOC(sizeof(sctable_entry_t));
  e->key = key;
  e->payload = value;
  if (t->entries >= t->resize_threshold)
      sctable_resize(t);
  sctable_insert(t, e);
}

void
sctable_add_or_replace(sctable_t *t, uint key, void *value)
{
  sctable_entry_t *e = sctable_lookup_entry(t, key);
  if (e != NULL)
    e->payload = value;
  else
    sctable_add(t, key, value);
}

void
sctable_remove(sctable_t *t, uint key)
{
    sctable_entry_t *e, *prev_e = NULL;
    uint hindex = HASH_FUNC(key, t);
    for (e = t->data[hindex]; e; prev_e = e, e = e->next) {
        if (e->key == key) {
            if (prev_e)
                prev_e->next = e->next;
            else
                t->data[hindex] = e->next;
            t->entries--;
            TABLE_FREE(e);
            break;
        }
    }
}

void
sctable_destroy(sctable_t *t)
{
  TABLE_FREE(t->data);
}
