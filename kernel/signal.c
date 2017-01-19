#include "signal.h"

#include "assert.h"
#include "mem.h"
#include "nvic.h"

void signal_init(signal_t *sig)
{
  ASSERT(sig != NULL);

  heap_create(&sig->wait_list);
  spinlock_init(&sig->lock);
}

void signal_wait(signal_t *sig)
{
  signal_wait_if(sig, NULL);
}

void signal_wait_if(signal_t *sig, bool (*condition)())
{
  signal_enqueue_if(sig, condition);

  task_yield();
}

void signal_enqueue(signal_t *sig)
{
  signal_enqueue_if(sig, NULL);
}

void signal_enqueue_if(signal_t *sig, bool (*condition)())
{
  ASSERT(sig != NULL);

  task_t *task = task_self();

  spinlock_acquire(&sig->lock);

  if (condition == NULL || condition()) {
    task_unschedule(task);

    heap_insert(&sig->wait_list, &task->queue.heap,
                task_get_priority(task)->deadline);
  }

  spinlock_release(&sig->lock);
}

/* internal version that doesn't lock */
static void signal_wake(signal_t *sig)
{
  if (!heap_empty(&sig->wait_list)) {
    task_t *task = containerof(heap_root(&sig->wait_list), task_t, queue.heap);
    heap_remove(&sig->wait_list, &task->queue.heap);
    task_schedule(task);
  }
}

void signal_wake_one(signal_t *sig)
{
  ASSERT(sig != NULL);

  spinlock_acquire(&sig->lock);

  signal_wake(sig);

  spinlock_release(&sig->lock);
}

void signal_wake_all(signal_t *sig)
{
  spinlock_acquire(&sig->lock);

  while (!heap_empty(&sig->wait_list))
    signal_wake(sig);

  spinlock_release(&sig->lock);
}
