#include "mem.h"

#include "assert.h"
#include "basic_uart.h"
#include "list.h"
#include "spinlock.h"
#include "types.h"

extern char __heap_start__;

void *__brk;
spinlock_t __free_list_lock;
list_t __free_list;
spinlock_t __alloc_list_lock;
list_t __alloc_list;

#define MAGIC1 0xab
#define MAGIC2 0x21

/* minimum size, in bytes, of a free block */
#define MIN_FREE_BLK_SIZE 8
/* maximum heap size */
#define MAX_HEAP_SIZE (30 * KB)

#define check_free_blk(b) \
  ASSERT((b)->magic1 == MAGIC1 && (b)->magic2 == MAGIC2)

#define check_alloc_blk(b) \
  ASSERT((b)->magic1 == MAGIC2 && (b)->magic2 == MAGIC1)

typedef struct PACKED _ctrlblk_t
{
  /* 0xab */
  u8 magic1;
  /* size of free space, does not include space for ctrlblk_t */
  size_t size;
  /* link in free/alloc list */
  list_node_t next;
  /* 0x21 */
  u8 magic2;
} ctrlblk_t;

static inline void init_free_blk(ctrlblk_t *blk, size_t size)
{
  blk->magic1 = MAGIC1;
  blk->magic2 = MAGIC2;
  blk->size = size;
}

static inline void init_alloc_blk(ctrlblk_t *blk, size_t size)
{
  blk->magic1 = MAGIC2;
  blk->magic2 = MAGIC1;
  blk->size = size;
}

/* moves __brk by amount, creates a new ctrlblk_t within the space
 * and adds the new block to __free_list
 *
 * does not take any semaphores */
static void heap_expand(size_t amount)
{
  if ((u8 *)__brk + amount > (u8 *)(&__heap_start__ + MAX_HEAP_SIZE))
    PANIC("mem: out of memory");

  ctrlblk_t *blk = __brk;
  __brk += amount;
  init_free_blk(blk, amount - sizeof(ctrlblk_t));
  list_add(&__free_list, &blk->next);
}

void mem_init()
{
  __brk = &__heap_start__;
  spinlock_init(&__free_list_lock);
  spinlock_init(&__alloc_list_lock);
  list_init(&__free_list);
  list_init(&__alloc_list);
  heap_expand(KB);
}

void * malloc(size_t size)
{
  ASSERT(size > 0);

  ctrlblk_t *ablk = NULL;

  spinlock_acquire(&__free_list_lock);

  while (ablk == NULL) {
    if (!list_empty(&__free_list)) {
      list_foreach(&__free_list, node) {
        ctrlblk_t *blk = containerof(node, ctrlblk_t, next);

        check_free_blk(blk);

        if (blk->size >= size) {
          /* Found a free block large enough. */
          ablk = blk;

          list_remove(&__free_list, node);

          if (blk->size - size > sizeof(ctrlblk_t) + MIN_FREE_BLK_SIZE) {
            /* size of free block after split */
            size_t blk_size = blk->size - size - sizeof(ctrlblk_t);
            /* new free block */
            blk = (ctrlblk_t *)((u8 *)ablk + sizeof(ctrlblk_t) + size);
            init_free_blk(blk, blk_size);

            ablk->size = size;

            list_add(&__free_list, &blk->next);
          }

          init_alloc_blk(ablk, ablk->size);

          spinlock_acquire(&__alloc_list_lock);
          list_add(&__alloc_list, &ablk->next);
          spinlock_release(&__alloc_list_lock);

          break;
        }
      }
    }

    /* No free block found. */
    if (ablk == NULL) {
      size_t esize = KB;
      if (size > esize)
        esize = size;
      heap_expand(esize + sizeof(ctrlblk_t));
    }
  }

  spinlock_release(&__free_list_lock);

  return (u8 *)ablk + sizeof(ctrlblk_t);
}

void * realloc(void *ptr, size_t size)
{
  ASSERT(ptr != NULL && size > 0);

  ctrlblk_t *blk = (ctrlblk_t *)((u8 *)ptr - sizeof(ctrlblk_t));

  check_alloc_blk(blk);

  if (blk->size < size) {
    void *n = malloc(size);
    memcpy(n, ptr, blk->size);
    free(ptr);
    ptr = n;
  }

  return ptr;
}

void * a_malloc(size_t size, u8 align)
{
  ASSERT(size > 0 && align > 0);

  u8 *ptr = malloc(size + align);
  u8 *align_ptr = (u8 *)((u32)(ptr + align) / align * align);
  *(align_ptr - 1) = (u8)(align_ptr - ptr);
  return align_ptr;
}

void free(void *ptr)
{
  ASSERT(ptr != NULL);

  ctrlblk_t *blk = (ctrlblk_t *)((u8 *)ptr - sizeof(ctrlblk_t));

  check_alloc_blk(blk);
  init_free_blk(blk, blk->size);

  spinlock_acquire(&__alloc_list_lock);
  list_remove(&__alloc_list, &blk->next);
  spinlock_release(&__alloc_list_lock);

  spinlock_acquire(&__free_list_lock);
  list_add(&__free_list, &blk->next);
  spinlock_release(&__free_list_lock);
}

void a_free(void *align_ptr)
{
  ASSERT(align_ptr != NULL);

  u8 *ptr = ((u8 *)align_ptr) - 1;
  free(((u8 *)align_ptr) - *ptr);
}

void memcpy(void *dst, const void *src, size_t size)
{
  ASSERT(dst != NULL && src != NULL);

  u8 *bsrc = (u8 *)src;
  u8 *bdst = (u8 *)dst;

  if (src == dst || size == 0)
    return;

  if (bdst > bsrc && bdst < bsrc + size) {
    /* dst overlaps src so we need to copy the array backwards */
    for (; size != 0; --size)
      bdst[size - 1] = bsrc[size - 1];
  }
  else {
    for (u32 i = 0; size != 0; ++i, --size)
      bdst[i] = bsrc[i];
  }
}

void memzero(void *ptr, size_t size)
{
  ASSERT(ptr != NULL);

  for (u32 i = 0; i < size; ++i)
    ((u8 *)ptr)[i] = 0;
}

size_t mem_free_size()
{
  size_t free_size = 0;

  spinlock_acquire(&__free_list_lock);

  list_foreach(&__free_list, node) {
    ctrlblk_t *cb = containerof(node, ctrlblk_t, next);
    free_size += sizeof(ctrlblk_t) + cb->size;
  }

  spinlock_release(&__free_list_lock);

  return free_size;
}

size_t mem_alloc_size()
{
  size_t alloc_size = 0;

  spinlock_acquire(&__alloc_list_lock);

  list_foreach(&__alloc_list, node) {
    ctrlblk_t *cb = containerof(node, ctrlblk_t, next);
    alloc_size += sizeof(ctrlblk_t) + cb->size;
  }

  spinlock_release(&__alloc_list_lock);

  return alloc_size;
}

static void print_ctrlblk(ctrlblk_t *blk)
{
  basic_uart_tx_str("\r\n\taddr: ");
  basic_uart_tx_u32((u32)blk);
  basic_uart_tx_str("\r\n\tmagic1: ");
  basic_uart_tx_u8(blk->magic1);
  basic_uart_tx_str("\r\n\tmagic2: ");
  basic_uart_tx_u8(blk->magic2);
  basic_uart_tx_str("\r\n\tsize: ");
  basic_uart_tx_u32(blk->size);
}

void print_mem_stats()
{
  basic_uart_tx_nl();
  basic_uart_tx_nl();

  basic_uart_tx_str("free list:");
  list_foreach(&__free_list, node) {
    print_ctrlblk(containerof(node, ctrlblk_t, next));
  }

  basic_uart_tx_str("\r\nalloc list:");
  list_foreach(&__alloc_list, node) {
    print_ctrlblk(containerof(node, ctrlblk_t, next));
  }
}



#ifdef TEST

#include "assert.h"
#include "bits.h"
#include "util.h"

void mem_test()
{
  UT_HEADER();

  /* offsetof */
  struct {
    u8 a;
    u32 b;
    u16 c;
    u8 d;
  } struct1;
  UT_ASSERT(offsetof(typeof(struct1), a) == 0);
  UT_ASSERT(offsetof(typeof(struct1), b) == 4);
  UT_ASSERT(offsetof(typeof(struct1), c) == 8);
  UT_ASSERT(offsetof(typeof(struct1), d) == 10);

  /* containerof */
  struct {
    u8 a;
    struct {
      u32 a;
      u32 b;
      u32 c;
    } struct3;
  } struct2;
  UT_ASSERT(containerof(&struct2.a, typeof(struct2), a) == &struct2);
  UT_ASSERT(containerof(&struct2.struct3.a,
                        typeof(struct2),
                        struct3.a) == &struct2);

#define CTRLBLK(p) ((ctrlblk_t *)((u8 *)(p) - sizeof(ctrlblk_t)))

  mem_init();
  UT_ASSERT(!list_empty(&__free_list)
            && list_empty(&__alloc_list)
            && __free_list == &((ctrlblk_t *)(&__heap_start__))->next
            && __brk == &__heap_start__ + KB);

  /* basic malloc test */
  void *p = malloc(1);
  ctrlblk_t *cb = CTRLBLK(p);
  UT_ASSERT(cb->magic1 == MAGIC2
            && cb->magic2 == MAGIC1
            && cb->size == 1);
  free(p);
  UT_ASSERT(cb->magic1 == MAGIC1
            && cb->magic2 == MAGIC2);

  /* expand the heap */
  mem_init();
  u8 *old_brk = __brk;
  cb = CTRLBLK(malloc(KB));
  u32 brk_delta = (u8 *)__brk - old_brk;
  UT_ASSERT(cb->size == KB
            && brk_delta == KB + sizeof(ctrlblk_t)
            && __alloc_list == &cb->next);

  /* large allocation */
  mem_init();
  old_brk = __brk;
  cb = CTRLBLK(malloc(20 * KB));
  brk_delta = (u8 *)__brk - old_brk;
  UT_ASSERT(cb->size == 20 * KB
            && brk_delta == 20 * KB + sizeof(ctrlblk_t)
            && __alloc_list == &cb->next
            && mem_alloc_size() == 20 * KB + sizeof(ctrlblk_t));

  /* MIN_FREE_BLK_SIZE is observed */
  mem_init();
  old_brk = __brk;
  cb = (ctrlblk_t *)(&__heap_start__);
  u32 size = cb->size - sizeof(ctrlblk_t) - MIN_FREE_BLK_SIZE - 1;
  p = malloc(size);
  UT_ASSERT(__brk == old_brk);
  free(p);
  p = malloc(size + 2);
  UT_ASSERT((u8 *)__brk > old_brk);
  u8 *bad_p = ((u8 *)p) + 1;
  UT_ASSERT_FAIL(free(bad_p));
  free(p);
  UT_ASSERT_FAIL(free(p));
  UT_ASSERT(list_empty(&__alloc_list));

  /* lots o' malloc and free */
  mem_init();
  old_brk = __brk;
  for (int i = 0; i < 10000; ++i)
    free(malloc(100));
  UT_ASSERT(old_brk == __brk
            && list_empty(&__alloc_list));

  /* check if any memory is leaked */
  mem_init();
  for (int s = 2049; s > 1; s -= 16)
    free(malloc(s));
  UT_ASSERT(list_empty(&__alloc_list));
  UT_ASSERT(mem_free_size() == (char *)__brk - &__heap_start__);

  /* alligned malloc and free */
  mem_init();
  for (u32 i = 1; i <= 128; i <<= 1) {
    p = a_malloc(100, i);
    UT_ASSERT(bits_clr(((u32)p), i - 1));
    a_free(p);
    UT_ASSERT(list_empty(&__alloc_list));
  }

  /* non-overlapping memcpy */
  u8 s1[10] = { [0 ... 9] = 1 };
  u8 d1[10] = { [0 ... 9] = 2 };
  memcpy(d1, s1, 10);
  for (int i = 0; i < 10; ++i)
    UT_ASSERT(d1[i] == 1);

  /* destination overlaps end of source */
  u8 b1[19] = { [0 ... 9] = 1, [10 ... 18] = 2 };
  u8 *s2 = &b1[0];
  u8 *d2 = &b1[9];
  memcpy(d2, s2, 10);
  for (int i = 0; i < 10; ++i)
    UT_ASSERT(d2[i] == 1);

  /* destination overlaps the beginning of source */
  u8 b2[19] = { [0 ... 8] = 2, [9 ... 18] = 1 };
  u8 *s3 = &b2[9];
  u8 *d3 = &b2[0];
  memcpy(d3, s3, 10);
  for (int i = 0; i < 10; ++i)
    UT_ASSERT(d3[i] == 1);

  /* destination and source are the same */
  u8 s4[10] = { [0 ... 9] = 1 };
  memcpy(s4, s4, 10);
  for (int i = 0; i < 10; ++i)
    UT_ASSERT(s4[i] == 1);

  u8 s5[10] = { [0 ... 9] = 1 };
  u8 d5[10] = { [0 ... 9] = 2 };
  memcpy(d5, s5, 0);
  for (int i = 0; i < 10; ++i)
    UT_ASSERT(d5[i] == 2);

  /* failure cases */
  UT_ASSERT_FAIL(memcpy(NULL, d5, 10));
  UT_ASSERT_FAIL(memcpy(d5, NULL, 10));

  memzero(s4, 10);
  for (int i = 0; i < 10; ++i)
    UT_ASSERT(s4[i] == 0);

  u8 b3[3] = { [0 ... 2] = 1 };
  memzero(b3, 0);
  for (int i = 0; i < 3; ++i)
    UT_ASSERT(b3[i] == 1);

  UT_ASSERT_FAIL(memzero(NULL, 10));

  /* realloc smaller */
  mem_init();
  u8 *b = malloc(10);
  for (int i = 0; i < 10; ++i)
    b[i] = i;
  u8 *a = realloc(b, 5);
  UT_ASSERT(a == b
            && CTRLBLK(a)->size == 10);
  for (int i = 0; i < 5; ++i)
    UT_ASSERT(a[i] == i);

  /* realloc larger */
  mem_init();
  b = malloc(10);
  for (int i = 0; i < 10; ++i)
    b[i] = i;
  a = realloc(b, 20);
  UT_ASSERT(a != b
            && CTRLBLK(a)->size == 20
            && mem_alloc_size() == 20 + sizeof(ctrlblk_t));
  for (int i = 0; i < 10; ++i)
    UT_ASSERT(a[i] == i);

  /* exhaust memory */
  mem_init();
  UT_ASSERT_FAIL(malloc(100 * KB));

  mem_init();
  UT_ASSERT_FAIL(malloc(0));
  UT_ASSERT_FAIL(a_malloc(0, 1));
  UT_ASSERT_FAIL(a_malloc(1, 0));
  UT_ASSERT_FAIL(realloc(NULL, 1));
  UT_ASSERT_FAIL(realloc(p, 0));
  UT_ASSERT_FAIL(free(NULL));
  UT_ASSERT_FAIL(a_free(NULL));

  UT_FOOTER();
}

#endif
