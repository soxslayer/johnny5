#pragma once

#include "types.h"

#define _bm(m,b) ((m) << (b))
#define _b(b) _bm(1, b)

#define bits_set(r, b) (((r) & (b)) == (b))
#define bits_clr(r, b) (((r) & (b)) == 0)

#define set_bits(r, b) ((r) |= (b))
#define clr_bits(r, b) ((r) &= ~(b))
#define set_field(r, b, m) \
({ \
  typeof(r) __r = (r); \
  typeof(m) __m = (m); \
  clr_bits(__r, __m); \
  set_bits(__r, (b)); \
  r = __r; \
})

#define while_bits_clr(r, b) while(bits_clr(r, b))
#define while_bits_set(r, b) while(bits_set(r, b))

int find_set(u32 i);
int find_clr(u32 i);
