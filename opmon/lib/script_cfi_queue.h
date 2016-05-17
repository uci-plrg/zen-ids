#ifndef _SCRIPT_CFI_QUEUE_H_
#define _SCRIPT_CFI_QUEUE_H_ 1

typedef struct _scqueue_t {
  size_t entry_prev_offset;
  size_t entry_next_offset;
  void *head;
  void *tail;
} scqueue_t;

#define SCQUEUE_INIT(q, entry_type, prev, next) \
  scqueue_init(q, offsetof(entry_type, prev), offsetof(entry_type, next))

void scqueue_init(scqueue_t *q, size_t entry_prev_offset, size_t entry_next_offset);
void scqueue_enqueue(scqueue_t *q, void *e);
void *scqueue_dequeue(scqueue_t *q);

#endif
