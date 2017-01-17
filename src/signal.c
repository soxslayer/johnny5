#include "signal.h"

#include "assert.h"
#include "mem.h"
#include "nvic.h"

void signal_init(signal_t *sig)
{
  ASSERT(sig != NULL);

  heap_init(&sig->wait_list);
}

void signal_wait(signal_t *sig)
{
  signal_enqueue(sig);

  task_yield();
}

void signal_enqueue(signal_t *sig)
{
  ASSERT(sig != NULL);

  task_t *task = task_self();

  nvic_disable_int();

  task_unschedule(task);

  heap_insert(&sig->wait_list, &task->queue.heap,
              task_get_priority(task)->deadline);

  nvic_enable_int();
}

void signal_wake_one(signal_t *sig)
{
  nvic_disable_int();

  if (!heap_empty(&sig->wait_list)) {
    task_t *task = containerof(heap_root(&sig->wait_list), task_t, queue.heap);
    heap_remove(&sig->wait_list, &task->queue.heap);
    task_schedule(task);
  }

  nvic_enable_int();
}

void signal_wake_all(signal_t *sig)
{
  nvic_disable_int();

  while (!heap_empty(&sig->wait_list))
    signal_wake_one(sig);

  nvic_enable_int();
}
