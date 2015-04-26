#include "script_cfi_array.h"

#define LEVEL1_SHIFT 0x12
#define LEVEL2_SHIFT 0xc
#define LEVEL3_SHIFT 6
#define LEVEL_MASK 0x3f
#define MAX_CAPACITY 0x1000000

typedef struct _scarray_iterator_t {
  scarray_t *array;
  scarray_data_t *block;
  void **item;
  uint index;
} scarray_iterator_t;

static scarray_iterator_t iterator; // not threadsafe!

static void expand_level(scarray_data_t *level)
{
  uint i;
  void **old_blocks = level->blocks;
  level->blocks = malloc(++level->item_count * sizeof(void *));
  for (i = 0; i < level->item_count-1; i++)
    level->blocks[i] = old_blocks[i];
  if (old_blocks != NULL)
    free(old_blocks);
}

static void expand_level2(scarray_data_t *level)
{
  scarray_data_t *new_sub_level = malloc(sizeof(scarray_data_t));
  new_sub_level->blocks = malloc(BLOCK_SIZE * sizeof(void *));
  new_sub_level->item_count = 0;
  expand_level(level);
  level->blocks[level->item_count-1] = new_sub_level;
}

static void expand(scarray_t *a)
{
  uint level1_index = (a->capacity >> LEVEL1_SHIFT) & LEVEL_MASK;
  uint level2_index = (a->capacity >> LEVEL2_SHIFT) & LEVEL_MASK;
  scarray_data_t *level1 = &a->data[level1_index];

  if (a->capacity >= MAX_CAPACITY) {
    PRINT("Error: maximum indexing capacity 0x%x exceeded!\n", MAX_CAPACITY);
    return;
  }

  if (level2_index >= level1->item_count) {
    scarray_data_t *new_level_2 = malloc(sizeof(scarray_data_t));
    memset(new_level_2, 0, sizeof(scarray_data_t));
    expand_level2(new_level_2);
    expand_level(level1);
    level1->blocks[level1->item_count-1] = new_level_2;
  } else {
    scarray_data_t *level2 = (scarray_data_t *)level1->blocks[level2_index];
    expand_level2(level2);
  }

  a->capacity += BLOCK_SIZE;
}

static scarray_data_t *get_block(scarray_t *a, uint index)
{
  uint level1_index = (index >> LEVEL1_SHIFT) & LEVEL_MASK;
  uint level2_index = (index >> LEVEL2_SHIFT) & LEVEL_MASK;
  uint level3_index = (index >> LEVEL3_SHIFT) & LEVEL_MASK;
  scarray_data_t *level1 = &a->data[level1_index];
  scarray_data_t *level2 = (scarray_data_t *)level1->blocks[level2_index];
  return (scarray_data_t *)level2->blocks[level3_index];
}

void scarray_init(scarray_t *a)
{
  a->capacity = BLOCK_SIZE;
  a->size = 0;
  memset(&a->data, 0, BLOCK_SIZE * sizeof(scarray_data_t));
  a->data[0].item_count = 1;
  a->data[0].blocks = malloc(sizeof(void *));
  a->data[0].blocks[0] = malloc(sizeof(scarray_data_t));
  memset(a->data[0].blocks[0], 0, sizeof(scarray_data_t));
  expand_level2(a->data[0].blocks[0]);
}

void scarray_destroy(scarray_t *a)
{
  uint i, j, k, l;
  uint level1_count = a->capacity >> 0x18;

  for (i = 0; i < level1_count; i++) {
    scarray_data_t *level1 = &a->data[i];
    for (j = 0; j < level1->item_count; j++) {
      scarray_data_t *level2 = level1->blocks[j];
      for (k = 0; k < level2->item_count; k++) {
        scarray_data_t *level3 = level2->blocks[k];
        for (l = 0; l < level3->item_count; l++) {
          free(level3->blocks[l]);
        }
      }
    }
  }
}

void scarray_append(scarray_t *a, void *e)
{
  scarray_data_t *tail_block;
  if (a->size == a->capacity && a->size % BLOCK_SIZE == 0) {
    expand(a);
  }
  tail_block = get_block(a, a->size);
  tail_block->blocks[tail_block->item_count++] = e;
  a->size++;
}

void *scarray_get(scarray_t *a, uint index)
{
  if (index >= a->size) {
    fprintf(stderr, "Error: scarray_t.get() index out of bounds!\n");
    return NULL;
  }

  return get_block(a, index)->blocks[index & LEVEL_MASK];
}

void *scarray_iterator_start(scarray_t *a)
{
  iterator.array = a;
  iterator.block = get_block(a, 0);
  iterator.item = &iterator.block->blocks[0];
  iterator.index = 1;
  return *iterator.item;
}

void *scarray_iterator_next()
{
  if (iterator.index == iterator.array->size)
    return NULL;

  if (iterator.item - iterator.block->blocks == BLOCK_SIZE) {
    iterator.block = get_block(iterator.array, iterator.index);
    iterator.item = &iterator.block->blocks[0];
  }
  iterator.index++;
  return *iterator.item;
}

void scarray_unit_test()
{
  uint i, j;
  char *buffer;

  PRINT("Script-CFI array test starting...\n");

  scarray_t a;

  scarray_init(&a);

  for (j = 0; j < 0x100; j++) {
    for (i = 0; i < 0x100; i++) {
      buffer = malloc(32);
      sprintf(buffer, "foo-%d.%d", j, i);
      scarray_append(&a, buffer);
    }
    for (i = 0; i < 0x100; i++) {
      buffer = malloc(32);
      sprintf(buffer, "bar-%d.%d", j, i);
      scarray_append(&a, buffer);
    }
  }

  for (i = 0x40; i < 0x10000; i++) {
    PRINT("Array has %s in position %d\n", (const char *) scarray_get(&a, i), i);
  }

  scarray_destroy(&a);

  PRINT("Script-CFI array test successful!\n");
}
