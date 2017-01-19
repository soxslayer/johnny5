#include "heap.h"

#include "assert.h"
#include "mem.h"

#define HEAP_INIT_SIZE 16

#define PARENT_IDX(i) (((i) - 1) / 2)
#define LEFT_CHILD_IDX(i) (((i) * 2) + 1)
#define RIGHT_CHILD_IDX(i) (((i) * 2) + 2)

static void upheap(heap_t *heap, u32 idx)
{
  while (idx != 0) {
    u32 parent_idx = PARENT_IDX(idx);

    if (heap->heap[parent_idx]->value > heap->heap[idx]->value) {
      heap_node_t *t = heap->heap[parent_idx];
      heap->heap[parent_idx] = heap->heap[idx];
      heap->heap[parent_idx]->pos = parent_idx;
      heap->heap[idx] = t;
      heap->heap[idx]->pos = idx;
      idx = parent_idx;
    }
    else
      break;
  }
}

static void downheap(heap_t *heap, u32 idx)
{
  while (LEFT_CHILD_IDX(idx) < heap->ins) {
    u32 l_idx = LEFT_CHILD_IDX(idx);
    u32 r_idx = RIGHT_CHILD_IDX(idx);

    u32 l = heap->heap[l_idx]->value;
    u32 r = heap->heap[r_idx]->value;
    u32 i = heap->heap[idx]->value;

    if (l < r && l < i) {
      heap_node_t *t = heap->heap[l_idx];
      heap->heap[l_idx] = heap->heap[idx];
      heap->heap[l_idx]->pos = l_idx;
      heap->heap[idx] = t;
      heap->heap[idx]->pos = idx;
      idx = l_idx;
    }
    else if (r < i) {
      heap_node_t *t = heap->heap[r_idx];
      heap->heap[r_idx] = heap->heap[idx];
      heap->heap[r_idx]->pos = r_idx;
      heap->heap[idx] = t;
      heap->heap[idx]->pos = idx;
      idx = r_idx;
    }
    else
      break;
  }
}

void heap_create(heap_t *heap)
{
  ASSERT(heap != NULL);

  heap->heap = malloc(HEAP_INIT_SIZE * sizeof(heap_node_t));
  heap->size = HEAP_INIT_SIZE;
  heap->ins = 0;
}

void heap_destroy(heap_t *heap)
{
  ASSERT(heap != NULL);

  free(heap->heap);
}

void heap_insert(heap_t *heap, heap_node_t *node, u32 value)
{
  ASSERT(heap != NULL && node != NULL);

  if (heap->ins == heap->size) {
    heap->size *= 2;
    heap->heap = realloc(heap->heap, heap->size * sizeof(heap_node_t *));;
  }

  heap->heap[heap->ins] = node;
  heap->heap[heap->ins]->value = value;
  heap->heap[heap->ins]->pos = heap->ins;
  upheap(heap, heap->ins++);
}

void heap_remove(heap_t *heap, heap_node_t *node)
{
  ASSERT(heap != NULL && node != NULL && heap_contains(heap, node));

  if (node->pos == heap->ins - 1)
    --heap->ins;
  else {
    heap->heap[node->pos] = heap->heap[heap->ins - 1];
    heap->heap[node->pos]->pos = node->pos;
    --heap->ins;
    downheap(heap, node->pos);
  }
}

heap_node_t * heap_root(heap_t *heap)
{
  ASSERT(heap != NULL);

  return heap->heap[0];
}

bool heap_empty(heap_t *heap)
{
  ASSERT(heap != NULL);

  return heap->ins == 0;
}

bool heap_contains(heap_t *heap, heap_node_t *node)
{
  ASSERT(heap != NULL && node != NULL);

  if (node->pos >= heap->ins)
    return false;

  return heap->heap[node->pos] == node;
}



#ifdef TEST

#include "util.h"

void heap_test()
{
  UT_HEADER();

  size_t alloc_size = mem_alloc_size();

  /* insert nodes in heap in reverse order then verify returned in correctly */
  heap_t h;
  heap_create(&h);
  heap_node_t nodes[50];
  for (int i = 0; i < 50; ++i) {
    heap_insert(&h, &nodes[i], 49 - i);
    UT_ASSERT(heap_contains(&h, &nodes[i]));

    for (int j = 0; j < i; ++j)
      UT_ASSERT(heap_contains(&h, &nodes[j]));
  }
  u32 val = 0;
  for (int i = 0; i < 50; ++i) {
    heap_node_t *n = heap_root(&h);
    UT_ASSERT((i == 0 && n->value == 0) || val < n->value);
    val = n->value;
    heap_remove(&h, n);
  }
  UT_ASSERT(heap_empty(&h));

  /* failure cases */
  UT_ASSERT_FAIL(heap_create(NULL));
  UT_ASSERT_FAIL(heap_remove(NULL, &nodes[0]));
  UT_ASSERT_FAIL(heap_remove(&h, NULL));
  UT_ASSERT_FAIL(heap_remove(&h, &nodes[0]));
  UT_ASSERT_FAIL(heap_destroy(NULL));
  UT_ASSERT_FAIL(heap_insert(&h, NULL, 0));
  UT_ASSERT_FAIL(heap_insert(NULL, &nodes[0], 0));
  UT_ASSERT_FAIL(heap_root(NULL));
  UT_ASSERT_FAIL(heap_contains(NULL, &nodes[0]));
  UT_ASSERT_FAIL(heap_contains(&h, NULL));

  heap_destroy(&h);

  UT_ASSERT(alloc_size == mem_alloc_size());

  UT_FOOTER();
}

#endif
