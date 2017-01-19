#pragma once

#include "intrinsic.h"
#include "types.h"

/* atomically decrement *i by d, return 0 on success */
u32 atomic_dec(u32 *i, u32 d);
/* atomically increment *i by d, return 0 on success */
u32 atomic_inc(u32 *i, u32 d);
/* atomically compare and swap *i with n if the current value is o,
 * return 0 on success */
u32 atomic_cas(u32 *i, u32 n, u32 o);

#define cbarrier() asm volatile("":::"memory")
#define mbarrier() cbarrier(); dsb()
