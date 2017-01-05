#pragma once

#include "types.h"

typedef u32 spinlock_t;

#define SPINLOCK_INIT 0

void spinlock_init(spinlock_t *spin);
void spinlock_acquire(spinlock_t *spin);
void spinlock_release(spinlock_t *spin);
