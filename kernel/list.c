#include "list.h"

#include "assert.h"
#include "mem.h"

void list_init(list_t *list)
{
  list_clear(list);
}

void list_add(list_t *list, list_node_t *node)
{
  ASSERT(list != NULL && node != NULL);

  if (*list == NULL) {
    *list = node;
    node->next = node;
    node->prev = node;

    return;
  }

  node->next = (list_node_t *)*list;
  node->prev = (*list)->prev;

  (*list)->prev->next = node;
  (*list)->prev = node;

  *list = node;
}

void list_remove(list_t *list, list_node_t *node)
{
  ASSERT(list != NULL && node != NULL);

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
  list_add(list, node);
}

void list_push_back(list_t *list, list_node_t *node)
{
  ASSERT(list != NULL && node != NULL);

  if (*list == NULL) {
    *list = node;

    return;
  }

  node->next = *list;
  node->prev = (*list)->prev;

  (*list)->prev->next = node;
  (*list)->prev = node;
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

list_node_t * list_front(list_t *list)
{
  ASSERT(list != NULL);

  return *list;
}

list_node_t * list_back(list_t *list)
{
  ASSERT(list != NULL);

  return (*list)->prev;
}

bool list_empty(list_t *list)
{
  ASSERT(list != NULL);

  return *list == NULL;
}

void list_clear(list_t *list)
{
  ASSERT(list != NULL);

  *list = NULL;
}



#ifdef TEST

#include "assert.h"
#include "util.h"

/* unit test */
void list_test()
{
  UT_HEADER();

  UT_ASSERT_FAIL(list_init(NULL));

  /* test list creation */
  list_t l1;
  list_init(&l1);
  UT_ASSERT(l1 == NULL
            && list_empty(&l1));

  /* failure cases */
  list_node_t n1;
  UT_ASSERT_FAIL(list_add(&l1, NULL));
  UT_ASSERT_FAIL(list_add(NULL, &n1));
  UT_ASSERT_FAIL(list_remove(&l1, NULL));
  UT_ASSERT_FAIL(list_remove(NULL, &n1));
  UT_ASSERT_FAIL(list_push_front(&l1, NULL));
  UT_ASSERT_FAIL(list_push_front(NULL, &n1));
  UT_ASSERT_FAIL(list_push_back(&l1, NULL));
  UT_ASSERT_FAIL(list_push_back(NULL, &n1));
  UT_ASSERT_FAIL(list_pop_front(NULL));
  UT_ASSERT_FAIL(list_pop_back(NULL));
  UT_ASSERT_FAIL(list_front(NULL));
  UT_ASSERT_FAIL(list_back(NULL));
  UT_ASSERT_FAIL(list_empty(NULL));
  UT_ASSERT_FAIL(list_clear(NULL));

  /* basic insertion */
  list_add(&l1, &n1);
  UT_ASSERT(l1 == &n1
            && !list_empty(&l1));

  list_node_t n2;
  list_add(&l1, &n2);
  UT_ASSERT(l1 == &n2
            && n1.next == &n2 && n1.prev == &n2
            && n2.next == &n1 && n2.prev == &n1);

  list_node_t n3;
  list_add(&l1, &n3);
  UT_ASSERT(l1 == &n3
            && n3.next == &n2 && n3.prev == &n1
            && n2.next == &n1 && n2.prev == &n3
            && n1.next == &n3 && n1.prev == &n2);

  list_node_t n4;
  list_add(&l1, &n4);
  UT_ASSERT(l1 == &n4
            && n4.prev == &n1 && n4.next == &n3
            && n3.prev == &n4 && n3.next == &n2
            && n2.prev == &n3 && n2.next == &n1
            && n1.prev == &n2 && n1.next == &n4);

  /* basic removal, removes front */
  list_remove(&l1, &n4);
  UT_ASSERT(l1 == &n3
            && n3.next == &n2 && n3.prev == &n1
            && n2.next == &n1 && n2.prev == &n3
            && n1.next == &n3 && n1.prev == &n2);

  list_remove(&l1, &n3);
  UT_ASSERT(l1 == &n2
            && n1.next == &n2 && n1.prev == &n2
            && n2.next == &n1 && n2.prev == &n1);

  list_remove(&l1, &n2);
  UT_ASSERT(l1 == &n1);

  list_remove(&l1, &n1);
  UT_ASSERT(l1 == NULL);

  /* front and back returns correct node */
  list_add(&l1, &n1);
  list_add(&l1, &n2);
  list_add(&l1, &n3);
  list_add(&l1, &n4);
  UT_ASSERT(list_front(&l1) == &n4
            && list_back(&l1) == &n1);

  /* clear and empty work */
  list_clear(&l1);
  UT_ASSERT(list_empty(&l1));

  /* remove middle node */
  list_clear(&l1);
  list_add(&l1, &n1);
  list_add(&l1, &n2);
  list_add(&l1, &n3);
  list_add(&l1, &n4);
  list_remove(&l1, &n2);
  UT_ASSERT(l1 == &n4
            && n4.next == &n3 && n4.prev == &n1
            && n3.next == &n1 && n3.prev == &n4
            && n1.next == &n4 && n1.prev == &n3);

  /* remove back node */
  list_clear(&l1);
  list_add(&l1, &n1);
  list_add(&l1, &n2);
  list_add(&l1, &n3);
  list_add(&l1, &n4);
  UT_ASSERT(list_back(&l1) == &n1);
  list_remove(&l1, &n1);
  UT_ASSERT(l1 == &n4
            && n4.next == &n3 && n4.prev == &n2
            && n3.next == &n2 && n3.prev == &n4
            && n2.next == &n4 && n2.prev == &n3);

  /* push_front works */
  list_clear(&l1);
  list_push_front(&l1, &n1);
  UT_ASSERT(l1 == &n1
            && !list_empty(&l1));

  list_push_front(&l1, &n2);
  UT_ASSERT(l1 == &n2
            && n1.next == &n2 && n1.prev == &n2
            && n2.next == &n1 && n2.prev == &n1);

  list_push_front(&l1, &n3);
  UT_ASSERT(l1 == &n3
            && n3.next == &n2 && n3.prev == &n1
            && n2.next == &n1 && n2.prev == &n3
            && n1.next == &n3 && n1.prev == &n2);

  list_push_front(&l1, &n4);
  UT_ASSERT(l1 == &n4
            && n4.prev == &n1 && n4.next == &n3
            && n3.prev == &n4 && n3.next == &n2
            && n2.prev == &n3 && n2.next == &n1
            && n1.prev == &n2 && n1.next == &n4);

  /* pop_front works */
  list_pop_front(&l1);
  UT_ASSERT(l1 == &n3
            && n3.next == &n2 && n3.prev == &n1
            && n2.next == &n1 && n2.prev == &n3
            && n1.next == &n3 && n1.prev == &n2);

  list_pop_front(&l1);
  UT_ASSERT(l1 == &n2
            && n1.next == &n2 && n1.prev == &n2
            && n2.next == &n1 && n2.prev == &n1);

  list_pop_front(&l1);
  UT_ASSERT(l1 == &n1);

  list_pop_front(&l1);
  UT_ASSERT(l1 == NULL);

  /* push_back works */
  list_push_back(&l1, &n1);
  UT_ASSERT(l1 == &n1);

  list_push_back(&l1, &n2);
  UT_ASSERT(l1 == &n1
            && n1.prev == &n2 && n1.next == &n2
            && n2.prev == &n1 && n2.next == &n1);

  list_push_back(&l1, &n3);
  UT_ASSERT(l1 == &n1
            && n1.prev == &n3 && n1.next == &n2
            && n2.prev == &n1 && n2.next == &n3
            && n3.prev == &n2 && n3.next == &n1);

  list_push_back(&l1, &n4);
  UT_ASSERT(l1 == &n1
            && n1.prev == &n4 && n1.next == &n2
            && n2.prev == &n1 && n2.next == &n3
            && n3.prev == &n2 && n3.next == &n4
            && n4.prev == &n3 && n4.next == &n1);

  /* pop_back works */
  list_clear(&l1);
  list_add(&l1, &n1);
  list_add(&l1, &n2);
  list_add(&l1, &n3);
  list_add(&l1, &n4);

  list_pop_back(&l1);
  UT_ASSERT(l1 == &n4
            && n4.prev == &n2 && n4.next == &n3
            && n3.prev == &n4 && n3.next == &n2
            && n2.prev == &n3 && n2.next == &n4);

  list_pop_back(&l1);
  UT_ASSERT(l1 == &n4
            && n4.prev == &n3 && n4.next == &n3
            && n3.prev == &n4 && n3.next == &n4);

  list_pop_back(&l1);
  UT_ASSERT(l1 == &n4
            && n4.prev == &n4 && n4.next == &n4);

  list_pop_back(&l1);
  UT_ASSERT(l1 == NULL);

  /* foreach works */
  list_clear(&l1);
  list_add(&l1, &n1);
  list_add(&l1, &n2);
  list_add(&l1, &n3);
  list_add(&l1, &n4);
  int mask = 0;
  int cnt = 0;
  list_foreach(&l1, node) {
    if (node == &n1)
      mask |= 1;
    else if (node == &n2)
      mask |= 2;
    else if (node == &n3)
      mask |= 4;
    else if (node == &n4)
      mask |= 8;

    ++cnt;
  }
  UT_ASSERT(cnt == 4 && mask == 0xf);

  list_clear(&l1);
  list_add(&l1, &n1);
  list_add(&l1, &n2);
  mask = 0;
  cnt = 0;
  list_foreach(&l1, node) {
    if (node == &n1)
      mask |= 1;
    else if (node == &n2)
      mask |= 2;
    else if (node == &n3)
      mask |= 4;
    else if (node == &n4)
      mask |= 8;

    ++cnt;
  }
  UT_ASSERT(cnt == 2 && mask == 3);

  list_clear(&l1);
  list_add(&l1, &n1);
  mask = 0;
  cnt = 0;
  list_foreach(&l1, node) {
    if (node == &n1)
      mask |= 1;
    else if (node == &n2)
      mask |= 2;
    else if (node == &n3)
      mask |= 4;
    else if (node == &n4)
      mask |= 8;

    ++cnt;
  }
  UT_ASSERT(cnt == 1 && mask == 1);

  list_clear(&l1);
  mask = 0;
  cnt = 0;
  list_foreach(&l1, node) {
    if (node == &n1)
      mask |= 1;
    else if (node == &n2)
      mask |= 2;
    else if (node == &n3)
      mask |= 4;
    else if (node == &n4)
      mask |= 8;

    ++cnt;
  }
  UT_ASSERT(cnt == 0 && mask == 0);

  UT_FOOTER();
}

#endif
