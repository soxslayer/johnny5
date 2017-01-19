#pragma once

#include "list.h"
#include "types.h"

typedef struct _buff_t
{
  list_node_t list;
  size_t buff_size;
  u32 read_ptr;
  size_t n_wrote;
  u8 *buff;
} buff_t;

void buff_create(buff_t *buff, size_t size);
void buff_destroy(buff_t *buff);
bool buff_full(buff_t *buff);
bool buff_empty(buff_t *buff);
size_t buff_write(buff_t *buff, const void *src, size_t size);
size_t buff_read(buff_t *buff, void *dst, size_t size);
void buff_reset(buff_t *buff);
