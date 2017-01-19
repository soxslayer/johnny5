#include "bits.h"

int find_set(u32 i)
{
  if (i == 0)
    return -1;

  int b = 31;

  if (i & 0x0000ffff) {
    i &= 0x0000ffff;
    b -= 16;
  }
  if (i & 0x00ff00ff) {
    i &= 0x00ff00ff;
    b -= 8;
  }
  if (i & 0x0f0f0f0f) {
    i &= 0x0f0f0f0f;
    b -= 4;
  }
  if (i & 0x33333333) {
    i &= 0x33333333;
    b -= 2;
  }
  if (i & 0x55555555)
    b -= 1;

  return b;
}

int find_clr(u32 i)
{
  return find_set(~i);
}
