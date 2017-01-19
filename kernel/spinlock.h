#pragma once

#include "list.h"
#include "task.h"
#include "types.h"

typedef struct _spinlock_t {
  u32 holder;
  list_t blocked_list;
} spinlock_t;

#define SPINLOCK_INIT 0

void spinlock_init(spinlock_t *spin);
void spinlock_acquire(spinlock_t *spin);
void spinlock_release(spinlock_t *spin);
