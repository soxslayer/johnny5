#pragma once

#include "heap.h"
#include "signal.h"
#include "spinlock.h"
#include "task.h"
#include "types.h"

typedef struct _sem_t
{
  spinlock_t guard;
  signal_t wait_list;
  u32 value;
  u32 max;
} sem_t;

void sem_init(sem_t *sem, u32 init_value, u32 max_value);
void sem_binary_init(sem_t *sem);
void sem_post();
void sem_take(sem_t *sem);
int sem_try_take(sem_t *sem);
void sem_give(sem_t *sem);
