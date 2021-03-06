#pragma once

#include "types.h"

#define NULL ((void *)0)

#define offsetof(st,m) __builtin_offsetof(st,m)
#define containerof(ptr,type,member) ({ \
  const __typeof__(((type *)0)->member) *__mptr = (ptr); \
  (type *)((char *)__mptr - offsetof(type,member)); \
  })

#define KB 1024

#define PACKED __attribute__((__packed__))
#define ALIGNED(x) __attribute__((aligned(x)))
#define SECTION(x) __attribute__((section(x)))

void mem_init();

void * malloc(size_t size);
void * realloc(void *ptr, size_t size);
void * a_malloc(size_t size, u8 align);
void free(void *ptr);
void a_free(void *align_ptr);
void memcpy(void *dst, const void *src, size_t size);
void memzero(void *ptr, size_t size);
size_t mem_free_size();
size_t mem_alloc_size();
void print_mem_stats();



#ifdef TEST
void mem_test();
#endif
