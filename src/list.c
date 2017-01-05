#include "list.h"

#include "assert.h"
#include "mem.h"

void list_init(list_t *list)
{
  ASSERT(list != NULL);

  *list = NULL;
}

void list_add(list_t *list, list_node_t *node)
{
  ASSERT(list != NULL);
  ASSERT(node != NULL);

  if (*list == NULL) {
    *list = node;
    node->next = node;
    node->prev = node;

    return;
  }

  node->next = (*list)->next;
  node->prev = (list_node_t *)*list;

  if ((*list)->next != NULL)
    (*list)->next->prev = node;

  (*list)->next = node;
}

void list_remove(list_t *list, list_node_t *node)
{
  ASSERT(list != NULL);
  ASSERT(node != NULL);

  if (*list == node) {
    *list = node->next;

    if (*list == node) {
      *list = NULL;
      return;
    }
  }

  node->prev->next = node->next;
  node->next->prev = node->prev;

  node->next = NULL;
  node->prev = NULL;
}

void list_push_front(list_t *list, list_node_t *node)
{
  if (*list == NULL) {
    list_add(list, node);

    return;
  }

  list_add(&(*list)->prev, node);
  *list = node;

  return;
}

void list_push_back(list_t *list, list_node_t *node)
{
  if (*list == NULL) {
    list_add(list, node);

    return;
  }

  list_add(&(*list)->prev, node);

  return;
}

void list_pop_front(list_t *list)
{
  ASSERT(list != NULL);

  list_remove(list, *list);
}

void list_pop_back(list_t *list)
{
  ASSERT(list != NULL);

  list_remove(list, (*list)->prev);
}

void list_pop_push_head(list_t *list)
{
  ASSERT(list != NULL);

  if (*list != NULL)
    *list = (*list)->next;
}

list_node_t * list_head(list_t *list)
{
  if (*list == NULL)
    return NULL;

  return *list;
}

list_node_t * list_tail(list_t *list)
{
  if (*list == NULL)
    return NULL;

  return (*list)->prev;
}

bool list_empty(list_t *list)
{
  ASSERT(list != NULL);

  return *list == NULL;
}
