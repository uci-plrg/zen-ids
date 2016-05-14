#include <stdlib.h>
#include "script_cfi_utils.h"
#include "script_cfi_queue.h"

/*
typedef struct _scqueue_t {
  size_t entry_prev_offset;
  size_t entry_next_offset;
  void *head;
  void *tail;
} scqueue_t;
*/

static inline void *entry_get_chain(scqueue_t *q, size_t entry_chain_offset, void *e)
{
  void **next_field = (void *) (((byte *) e) + entry_chain_offset);
  return *next_field;
}

static inline void entry_set_chain(scqueue_t *q, void *e, size_t entry_chain_offset, void *target)
{
  void **next_field = (void *) (((byte *) e) + entry_chain_offset);
  *next_field = target;
}

void scqueue_init(scqueue_t *q, size_t entry_prev_offset, size_t entry_next_offset)
{
  memset(q, 0, sizeof(scqueue_t));

  q->entry_prev_offset = entry_prev_offset;
  q->entry_next_offset = entry_next_offset;
}

void scqueue_enqueue(scqueue_t *q, void *e)
{
  if (q->head != NULL)
    entry_set_chain(q, q->head, q->entry_prev_offset, e);
  entry_set_chain(q, e, q->entry_next_offset, q->head);
  q->head = e;
  entry_set_chain(q, e, q->entry_prev_offset, NULL);

  if (q->tail == NULL)
    q->tail = q->head;
}

void *scqueue_dequeue(scqueue_t *q)
{
  void *e = q->tail;

  if (q->tail != NULL) {
    if (q->head == q->tail)
      q->head = q->tail = NULL;
    else
      q->tail = entry_get_chain(q, q->entry_prev_offset, q->tail);
  }

  return e;
}
