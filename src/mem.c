#include "mem.h"

#include "assert.h"
#include "basic_uart.h"
#include "list.h"
#include "sem.h"
#include "types.h"

extern char __heap_start__;

void *__brk;
sem_t __free_list_sem = BINARY_SEM_INIT;
list_t __free_list;
sem_t __alloc_list_sem = BINARY_SEM_INIT;
list_t __alloc_list;

#define MAGIC1 0xab
#define MAGIC2 0x21

/* minimum size, in bytes, of a free block */
#define MIN_FREE_BLK_SIZE 8
/* maximum heap size */
#define MAX_HEAP_SIZE (30 * KB)

#define check_blk(b) \
  if ((b)->magic1 != MAGIC1 || (b)->magic2 != MAGIC2) { \
    PANIC("mem: heap corruption"); \
  }

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

static void init_blk(ctrlblk_t *blk, size_t size)
{
  blk->magic1 = MAGIC1;
  blk->magic2 = MAGIC2;
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
  init_blk(blk, amount - sizeof(ctrlblk_t));
  list_add(&__free_list, &blk->next);
}

void mem_init()
{
  __brk = &__heap_start__;
  list_init(&__free_list);
  list_init(&__alloc_list);
  heap_expand(KB);
}

void *malloc(size_t size)
{
  ctrlblk_t *ablk = NULL;

  sem_take(&__free_list_sem);

  while (ablk == NULL) {
    if (__free_list != NULL) {
      list_foreach(&__free_list, node) {
        ctrlblk_t *blk = containerof(node, ctrlblk_t, next);

        check_blk(blk);

        if (blk->size >= size) {
          /* Found a free block large enough. */
          ablk = blk;

          list_remove(&__free_list, node);

          if (blk->size - size > sizeof(ctrlblk_t) + MIN_FREE_BLK_SIZE) {
            /* size of free block after split */
            size_t blk_size = blk->size - size - sizeof(ctrlblk_t);
            /* new free block */
            blk = (ctrlblk_t *)((u8 *)ablk + sizeof(ctrlblk_t) + size);

            init_blk(blk, blk_size);

            ablk->size = size;

            list_add(&__free_list, &blk->next);
          }

          sem_take(&__alloc_list_sem);
          list_add(&__alloc_list, &ablk->next);
          sem_give(&__alloc_list_sem);

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

  sem_give(&__free_list_sem);

  return (u8 *)ablk + sizeof(ctrlblk_t);
}

void *a_malloc(size_t size, u8 align)
{
  u8 *ptr = malloc(size + align);
  u8 *align_ptr = (u8 *)((u32)(ptr + align) / align * align);
  *(align_ptr - 1) = (u8)(align_ptr - ptr);
  return align_ptr;
}

void free(void *ptr)
{
  ctrlblk_t *blk = (ctrlblk_t *)((u8 *)ptr - sizeof(ctrlblk_t));

  check_blk(blk);

  sem_take(&__alloc_list_sem);
  list_remove(&__alloc_list, &blk->next);
  sem_give(&__alloc_list_sem);

  sem_take(&__free_list_sem);
  list_add(&__free_list, &blk->next);
  sem_give(&__free_list_sem);
}

void a_free(void *align_ptr)
{
  ASSERT(align_ptr != NULL);

  u8 *ptr = ((u8 *)align_ptr) - 1;
  free(((u8 *)align_ptr) - *ptr);
}

void memcpy(void *dst, void *src, size_t size)
{
  u8 *bsrc = (u8 *)src;
  u8 *bdst = (u8 *)dst;

  if (src == dst || size == 0)
    return;

  if (bdst > bsrc && bdst < bsrc + size) {
    /* dst overlaps src so we need to copy the array backwards */
    for (u32 i = size - 1; size != 0; --i, --size)
      bdst[i] = bsrc[i];
  }
  else {
    for (u32 i = 0; size != 0; ++i, --size)
      bdst[i] = bsrc[i];
  }
}

void memzero(void *ptr, size_t size)
{
  if (ptr == NULL)
    return;

  for (u32 i = 0; i < size; ++i)
    ((u8 *)ptr)[i] = 0;
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
