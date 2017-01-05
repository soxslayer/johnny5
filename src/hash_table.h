#pragma once

#include "list.h"
#include "types.h"

typedef list_node_t hash_table_node_t;

typedef struct _hash_table_t
{
  list_t *table;
  size_t table_size;
  void (*key_func)(hash_table_node_t *, void **key, size_t *key_size);
} hash_table_t;

u32 binary_hash_func(void *ptr, size_t size);

void hash_table_create(hash_table_t *table, size_t table_size,
  void (*key_func)(hash_table_node_t *, void **, size_t *));
void hash_table_destroy(hash_table_t *table);
void hash_table_insert(hash_table_t *table, hash_table_node_t *node);
void hash_table_remove(hash_table_t *table, hash_table_node_t *node);
hash_table_node_t * hash_table_get(hash_table_t *table, const void *key,
                                   size_t key_size);
bool hash_table_contains(hash_table_t *table, const void *key, size_t key_size);
