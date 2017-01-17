#include "spinlock.h"

#include "assert.h"
#include "atomic.h"
#include "intrinsic.h"
#include "nvic.h"
#include "task.h"

void spinlock_init(spinlock_t *spin)
{
  ASSERT(spin != NULL);

  spin->holder = 0;
  list_init(&spin->blocked_list);
}

void spinlock_acquire(spinlock_t *spin)
{
  ASSERT(spin != NULL);

  mbarrier();

  task_t *self = task_self();

  while (!atomic_cas(&spin->holder, (u32)self, 0)) {
    nvic_disable_int();

    list_add(&spin->blocked_list, &self->spinlock_node);
    task_blocked((task_t *)spin->holder);

    nvic_enable_int();

    task_yield();
  }

  mbarrier();
}

void spinlock_release(spinlock_t *spin)
{
  ASSERT(spin != NULL);

  mbarrier();

  task_t *self = task_self();

  atomic_cas(&spin->holder, 0, (u32)self);

  mbarrier();

  nvic_disable_int();

  list_foreach(&spin->blocked_list, node) {
    task_t *t = containerof(node, task_t, spinlock_node);
    task_unblocked(t);
  }
  list_clear(&spin->blocked_list);

  nvic_enable_int();
}
