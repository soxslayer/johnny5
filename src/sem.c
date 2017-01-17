#include "sem.h"

#include "assert.h"
#include "atomic.h"
#include "intrinsic.h"
#include "mem.h"
#include "nvic.h"
#include "task.h"

static int sem_take_post(sem_t *sem, bool block)
{
  spinlock_acquire(&sem->guard);

  while (sem->value == 0) {
    if (!block) {
      spinlock_release(&sem->guard);
      return 0;
    }

    /* a context switch here could deadlock, so disable ints */
    nvic_disable_int();
    signal_enqueue(&sem->wait_list);
    spinlock_release(&sem->guard);
    nvic_enable_int();
    task_yield();
    spinlock_acquire(&sem->guard);
  }

  --sem->value;

  mbarrier();

  spinlock_release(&sem->guard);

  return 1;
}

static void sem_give_post(sem_t *sem)
{
  spinlock_acquire(&sem->guard);

  ASSERT(sem->max == -1 || sem->value <= sem->max - 1);

  ++sem->value;

  mbarrier();

  signal_wake_one(&sem->wait_list);

  spinlock_release(&sem->guard);
}

static int sem_take_pre(sem_t *sem, bool block)
{
  if (sem->value == 0)
    return 0;

  if (sem->value > 0)
    --sem->value;

  mbarrier();

  return 1;
}

static void sem_give_pre(sem_t *sem)
{
  ASSERT(sem->max == -1 || sem->value <= sem->max - 1);

  ++sem->value;

  mbarrier();
}

int (*__sem_take)(sem_t *sem, bool) = sem_take_pre;
void (*__sem_give)(sem_t *sem) = sem_give_pre;

void sem_init(sem_t *sem, u32 init_value, u32 max_value)
{
  ASSERT(sem != NULL);

  signal_init(&sem->wait_list);
  sem->value = init_value;
  spinlock_init(&sem->guard);
  sem->max = max_value;
}

void sem_binary_init(sem_t *sem)
{
  sem_init(sem, 1, 1);
}

void sem_post()
{
  __sem_take = sem_take_post;
  __sem_give = sem_give_post;
}

void sem_take(sem_t *sem)
{
  ASSERT(sem != NULL);

  __sem_take(sem, true);
}

int sem_try_take(sem_t *sem)
{
  ASSERT(sem != NULL);

  return __sem_take(sem, false);
}

void sem_give(sem_t *sem)
{
  ASSERT(sem != NULL);

  __sem_give(sem);
}
