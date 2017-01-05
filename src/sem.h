#pragma once

#include "signal.h"
#include "spinlock.h"
#include "types.h"

/* static initializers, no need to call sem_init with these */
#define BINARY_SEM_INIT { \
  .guard = SPINLOCK_INIT, \
  .wait_list = SIGNAL_INIT, \
  .value = 1, \
  .max = 1 \
}

#define SEM_INIT(iv) { \
  .guard = SPINLOCK_INIT, \
  .wait_list = SIGNAL_INIT, \
  .value = iv, \
  .max = -1 \
}

typedef struct _sem_t
{
  spinlock_t guard;
  signal_t wait_list;
  u32 value;
  u32 max;
} sem_t;

void sem_post();
void sem_init(sem_t *sem, u32 init_value, u32 max_value);
void sem_take(sem_t *sem);
void sem_give(sem_t *sem);
