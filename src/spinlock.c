#include "spinlock.h"

#include "assert.h"
#include "atomic.h"
#include "intrinsic.h"
#include "task.h"

void spinlock_init(spinlock_t *spin)
{
  ASSERT(spin != NULL);

  spin = 0;
}

void spinlock_acquire(spinlock_t *spin)
{
  ASSERT(spin != NULL);

  mbarrier();

  while (!atomic_cas(spin, 1, 0))
    task_yield();

  mbarrier();
}

void spinlock_release(spinlock_t *spin)
{
  ASSERT(spin != NULL);

  mbarrier();

  atomic_cas(spin, 0, 1);

  mbarrier();
}
