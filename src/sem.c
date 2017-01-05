#include "sem.h"

#include "assert.h"
#include "atomic.h"
#include "intrinsic.h"
#include "mem.h"
#include "nvic.h"
#include "task.h"

static void sem_take_post(sem_t *sem)
{
  spinlock_acquire(&sem->guard);

  while (sem->value == 0) {
    /* a context switch here could deadlock, so disable ints */
    nvic_disable_int();
    signal_enqueue(&sem->wait_list);
    spinlock_release(&sem->guard);
    nvic_enable_int();
    task_yield();
    spinlock_acquire(&sem->guard);
  }

  --sem->value;
  dsb();
  spinlock_release(&sem->guard);
}

static void sem_give_post(sem_t *sem)
{
  spinlock_acquire(&sem->guard);

  ASSERT(sem->max == -1 || sem->value <= sem->max - 1);

  ++sem->value;
  dsb();

  signal_wake_one(&sem->wait_list);

  spinlock_release(&sem->guard);
}

static void sem_take_pre(sem_t *sem)
{
  if (sem->value > 0)
    --sem->value;
}

static void sem_give_pre(sem_t *sem)
{
  ASSERT(sem->max == -1 || sem->value <= sem->max - 1);

  ++sem->value;
}

void (*__sem_take)(sem_t *sem) = sem_take_pre;
void (*__sem_give)(sem_t *sem) = sem_give_pre;

void sem_post()
{
  __sem_take = sem_take_post;
  __sem_give = sem_give_post;
}

void sem_init(sem_t *sem, u32 init_value, u32 max_value)
{
  ASSERT(sem != NULL);

  signal_init(&sem->wait_list);
  sem->value = init_value;
  spinlock_init(&sem->guard);
  sem->max = max_value;
}

void sem_take(sem_t *sem)
{
  ASSERT(sem != NULL);

  __sem_take(sem);
}

void sem_give(sem_t *sem)
{
  ASSERT(sem != NULL);

  __sem_give(sem);
}
