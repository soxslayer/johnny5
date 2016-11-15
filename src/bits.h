#pragma once

#define _bm(m,b) ((m) << (b))
#define _b(b) _bm(1, b)

#define bits_set(r, b) (((r) & (b)) == (b))
#define bits_clr(r, b) (((r) & (b)) == 0)

#define while_bits_clr(r, b) while(bits_clr(r, b))
#define while_bits_set(r, b) while(bits_set(r, b))
