#include "script_cfi_array.h"

#define BASE_CAPACITY 0x10

struct _scarray_iterator_t {
  scarray_t *array;
  void **item;
  uint index;
};

static void expand(scarray_t *a)
{
  void **new_data = malloc((a->capacity * 2) * sizeof(void *));
  memcpy(new_data, a->data, a->capacity * sizeof(void *));
  free(a->data);
  a->data = new_data;
  a->capacity *= 2;
}

void scarray_init(scarray_t *a)
{
  a->capacity = BASE_CAPACITY;
  a->size = 0;
  a->data = malloc(a->capacity * sizeof(void *));
  memset(a->data, 0, a->capacity * sizeof(void *));
}

void scarray_destroy(scarray_t *a)
{
  free(a->data);
}

void scarray_append(scarray_t *a, void *e)
{
  if (a->size == a->capacity)
    expand(a);
  a->data[a->size++] = e;
}

void *scarray_get(scarray_t *a, uint index)
{
  return a->data[index];
}

void *scarray_remove(scarray_t *a, uint index)
{
  void *removal = a->data[index];

  a->size--;

  if (index < a->size)
    memmove(&a->data[index], &a->data[index+1], (a->size - index) * sizeof(void *));

  return removal;
}

scarray_iterator_t *scarray_iterator_start(scarray_t *a)
{
  scarray_iterator_t *iterator = malloc(sizeof(scarray_iterator_t));
  iterator->array = a;
  iterator->item = &iterator->array->data[0];
  iterator->index = 0;
  return iterator;
}

scarray_iterator_t *scarray_iterator_start_at(scarray_t *a, uint start_index)
{
  scarray_iterator_t *iterator = scarray_iterator_start(a);
  iterator->item = &iterator->array->data[start_index];
  iterator->index = start_index;
  return iterator;
}

void *scarray_iterator_next(scarray_iterator_t *iterator)
{
  void *item;

  if (iterator->index++ >= iterator->array->size)
    return NULL;

  item = *iterator->item;
  iterator->item++;
  return item;
}

uint scarray_iterator_index(scarray_t *a, scarray_iterator_t *i)
{
  if (i->item > a->data && i->item < (a->data + a->size))
    return (uint) (i->item - a->data);
  else
    return SCARRAY_ERROR_INVALID_ELEMENT;
}

void scarray_iterator_end(scarray_iterator_t *iterator)
{
  free(iterator);
}

void scarray_unit_test()
{
  uint i, j;
  char *buffer;
  scarray_iterator_t *iter;

  SPOT("Script-CFI array test starting...\n");

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
    SPOT("Array has %s in position %d\n", (const char *) scarray_get(&a, i), i);
  }

  i = 0;
  iter = scarray_iterator_start(&a);
  while ((buffer = (char *) scarray_iterator_next(iter)) != NULL) {
    if (i == 63)
      SPOT("stop here\n");
    SPOT("Free element %s\n", buffer);
    free(buffer);
  }
  scarray_iterator_end(iter);

  scarray_destroy(&a);

  SPOT("Script-CFI array test successful!\n");
}
