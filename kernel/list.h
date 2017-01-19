#pragma once

#include "mem.h"
#include "types.h"

#define LIST_INIT NULL

typedef struct _list_node_t
{
  struct _list_node_t *next;
  struct _list_node_t *prev;
} list_node_t;

typedef list_node_t * list_t;

void list_init(list_t *list);
void list_add(list_t *list, list_node_t *node);
void list_remove(list_t *list, list_node_t *node);
void list_push_front(list_t *list, list_node_t *node);
void list_push_back(list_t *list, list_node_t *node);
void list_pop_front(list_t *list);
void list_pop_back(list_t *list);
list_node_t * list_front(list_t *list);
list_node_t * list_back(list_t *list);
bool list_empty(list_t *list);
void list_clear(list_t *list);

#define list_foreach(l,v) \
  for (list_node_t *v = *(l), *__h = NULL, *__n = v->next; \
       (__h == NULL || v != *(l)) && *(l) != NULL; \
       __h = v, v = __n, __n = __n->next)



#ifdef TEST
void list_test();
#endif
