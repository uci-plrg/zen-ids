#include "script_cfi_hashtable.h"

#define KEY_TYPE uint64
#define HASH_TAG_BITS ((sizeof(KEY_TYPE)) * 8)
#define UINT_0 ((KEY_TYPE) 0U)

#define HASH_FUNC(key, table) (((key) ^ ((key) >> 8)) & (table)->hash_mask)
#define HASH_MASK(num_bits) ((~UINT_0) >> (HASH_TAG_BITS-(num_bits)))
#define HASHTABLE_SIZE(num_bits) (1U << (num_bits))

#define TABLE_NEW(type) (type *) scalloc(sizeof(type), ALLOC_PROCESS)
#define TABLE_ALLOC(size) scalloc(size, ALLOC_PROCESS)
#define TABLE_FREE(ptr) scfree_process(ptr)

static void
sctable_insert(sctable_t *t, sctable_entry_t *e)
{
  uint hindex = HASH_FUNC(e->key, t);
  if (e == t->data[hindex]) {
    ERROR("Attempt to insert existing element in the hashtable!\n");
    return;
  }
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

    TABLE_FREE(old_data);
}

static sctable_entry_t *
sctable_lookup_entry(sctable_t *t, KEY_TYPE key)
{
    sctable_entry_t *e;
    uint hindex = HASH_FUNC(key, t);
    for (e = t->data[hindex]; e; e = e->next) {
        if (e->key == key)
            return e;
    }
    return NULL;
}

static inline
sctable_entry_t *sctable_new_entry(sctable_t *t)
{
  sctable_entry_t *e;

  if (t->entry_pool == NULL) {
    e = TABLE_NEW(sctable_entry_t);
  } else {
    if (t->entry_free_list->size > 0) {
      e = t->entry_free_list->data[--t->entry_free_list->size];
    } else if (t->entry_pool_index < t->entry_pool->size) {
      e = t->entry_pool->data[t->entry_pool_index++];
    } else {
      e = TABLE_NEW(sctable_entry_t);
      scarray_append(t->entry_pool, e);
      t->entry_pool_index++;
    }
  }

  return e;
}

static inline void
sctable_delete_entry(sctable_t *t, sctable_entry_t *e)
{
  if (t->entry_pool == NULL) {
    TABLE_FREE(e);
  } else {
    scarray_append(t->entry_free_list, e);
  }
}

static inline void
sctable_clear_entry_pool(sctable_t *t)
{
  if (t->entry_pool != NULL) {
    t->entry_pool_index = 0;
    t->entry_free_list->size = 0;
  }
}

/******************************************************************
 * Public API
 */

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

void
sctable_activate_pool(sctable_t *t)
{
  if (t->entry_pool != NULL) {
    ERROR("sctable entry pool is already active!\n");
    return;
  }
  if (t->entries > 0) {
    ERROR("sctable entry pool cannot be activated while entries exist in the table!\n");
    return;
  }

  t->entry_pool = TABLE_NEW(scarray_t);
  scarray_init(t->entry_pool);
  t->entry_free_list = TABLE_NEW(scarray_t);
  scarray_init(t->entry_free_list);
}

void *
sctable_lookup(sctable_t *t, KEY_TYPE key)
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
sctable_add(sctable_t *t, KEY_TYPE key, void *value)
{
  sctable_entry_t *e = sctable_new_entry(t);
  e->key = key;
  e->payload = value;
  if (t->entries >= t->resize_threshold)
      sctable_resize(t);
  sctable_insert(t, e);
}

void
sctable_add_or_replace(sctable_t *t, KEY_TYPE key, void *value)
{
  sctable_entry_t *e = sctable_lookup_entry(t, key);
  if (e != NULL)
    e->payload = value;
  else
    sctable_add(t, key, value);
}

void *
sctable_remove(sctable_t *t, KEY_TYPE key)
{
  void *payload = NULL;
  sctable_entry_t *e, *prev_e = NULL;
  uint hindex = HASH_FUNC(key, t);
  for (e = t->data[hindex]; e; prev_e = e, e = e->next) {
    if (e->key == key) {
      if (prev_e)
        prev_e->next = e->next;
      else
        t->data[hindex] = e->next;
      t->entries--;
      payload = e->payload;
      sctable_delete_entry(t, e);
      break;
    }
  }
  return payload;
}

void
sctable_clear(sctable_t *t)
{
  uint i;
  sctable_entry_t *e, *next;
  for (i = 0; i < t->capacity; i++) {
    for (e = t->data[i]; e; e = next) {
      next = e->next;
      t->entries--;
      sctable_delete_entry(t, e);
    }
    t->data[i] = NULL;
  }
  sctable_clear_entry_pool(t);

  if (t->entries != 0)
    ERROR("Failed to clear the hashtable!\n");
}

void
sctable_erase(sctable_t *t)
{
  if (t->entry_pool == NULL) {
    WARN("Cannot erase an sctable with inactive entry pool--clearing instead.\n");
    sctable_clear(t);
    return;
  }

  t->entries = 0;
  memset(t->data, 0, t->capacity * sizeof(sctable_entry_t *));
  sctable_clear_entry_pool(t);
}

void
sctable_destroy(sctable_t *t)
{
  TABLE_FREE(t->data);
}

#undef KEY_TYPE
