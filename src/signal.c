#include "signal.h"

#include "assert.h"
#include "mem.h"
#include "nvic.h"

void signal_init(signal_t *sig)
{
  ASSERT(sig != NULL);

  list_init(&sig->wait_list);
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

  list_add(&sig->wait_list, &task->queue.wait);

  nvic_enable_int();
}

void signal_wake_one(signal_t *sig)
{
  nvic_disable_int();

  if (!list_empty(&sig->wait_list)) {
    task_t *task = containerof(list_head(&sig->wait_list), task_t, queue.wait);
    list_remove(&sig->wait_list, &task->queue.wait);
    task_schedule(task);
  }

  nvic_enable_int();
}

void signal_wake_all(signal_t *sig)
{
  nvic_disable_int();

  list_foreach(&sig->wait_list, node) {
    signal_wake_one(sig);
  }

  nvic_enable_int();
}
