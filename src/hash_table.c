#include "hash_table.h"

#include "assert.h"
#include "mem.h"

static u32 hash_key(hash_table_t *table, const void *key, size_t key_size)
{
  u32 hash = 0;

  for (size_t i = 0; i < key_size; ++i)
    hash += ((u8 *)key)[i];

  return hash % table->table_size;
}

static u32 hash_entry(hash_table_t *table, hash_table_node_t *node)
{
  void *key;
  size_t key_size;
  table->key_func(node, &key, &key_size);

  return hash_key(table, key, key_size);
}

static bool compare_key(void *k1, size_t s1, void *k2, size_t s2)
{
  if (s1 != s2)
    return false;

  for (size_t i = 0; i < s1; ++i) {
    if (((u8 *)k1)[i] != ((u8 *)k2)[i])
      return false;
  }

  return true;
}

void hash_table_create(hash_table_t *table, size_t table_size,
  void (*key_func)(hash_table_node_t *, void **, size_t *))
{
  ASSERT(table != NULL);

  table->table_size = table_size;
  table->key_func = key_func;

  table->table = malloc(table_size * sizeof(list_t));
  for (size_t i = 0; i < table_size; ++i)
    list_init(&table->table[i]);
}

void hash_table_destroy(hash_table_t *table)
{
  ASSERT(table != NULL);

  free(table->table);
}

void hash_table_insert(hash_table_t *table, hash_table_node_t *node)
{
  ASSERT(table != NULL);
  ASSERT(node != NULL);

  u32 hash = hash_entry(table, node);
  list_add(&table->table[hash], (list_node_t *)node);
}

void hash_table_remove(hash_table_t *table, hash_table_node_t *node)
{
  ASSERT(table != NULL);
  ASSERT(node != NULL);

  u32 hash = hash_entry(table, node);
  list_remove(&table->table[hash], (list_node_t *)node);
}

hash_table_node_t * hash_table_get(hash_table_t *table, const void *key,
                                   size_t key_size)
{
  ASSERT(table != NULL);
  ASSERT(key != NULL);
  ASSERT(key_size > 0);

  u32 hash = hash_key(table, key, key_size);

  list_foreach(&table->table[hash], node) {
    hash_table_node_t *hnode = (hash_table_node_t *)node;
    void *key;
    size_t key_size;
    table->key_func(node, &key, &key_size);

    if (compare_key(key, key_size, key, key_size))
      return hnode;
  }

  return NULL;
}

bool hash_table_contains(hash_table_t *table, const void *key, size_t key_size)
{
  return hash_table_get(table, key, key_size) != NULL;
}
