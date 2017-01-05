#include "string.h"

size_t strncpy(char *dst, const char *src, size_t size)
{
  size_t r = 0;

  for (; size - 1 > 0 && *src != 0; ++r, --size, ++src, ++dst)
    *dst = *src;

  dst[r] = 0;

  return r;
}

size_t strnlen(const char *str, size_t max_size)
{
  size_t len = 0;

  for (len = 0; len < max_size; ++len) {
    if (str[len] == 0) {
      ++len;
      break;
    }
  }

  return len;
}
