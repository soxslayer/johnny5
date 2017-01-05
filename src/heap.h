#pragma once

#include "list.h"
#include "types.h"

typedef struct _heap_node_t
{
  u32 value;
  u32 pos;
} heap_node_t;

typedef struct _heap_t
{
  heap_node_t **heap;
  size_t size;
  u32 ins;
} heap_t;

void heap_init(heap_t *heap);
void heap_insert(heap_t *heap, heap_node_t *node, u32 value);
void heap_remove(heap_t *heap, heap_node_t *node);
heap_node_t * heap_root(heap_t *heap);
bool heap_empty(heap_t *heap);
