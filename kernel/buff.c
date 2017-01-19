#include "buff.h"

#include "assert.h"
#include "mem.h"

void buff_create(buff_t *buff, size_t size)
{
  ASSERT(buff != NULL && size > 0);

  buff->buff = malloc(size);
  buff->buff_size = size;
}

void buff_destroy(buff_t *buff)
{
  ASSERT(buff != NULL);

  free(buff->buff);
}

bool buff_full(buff_t *buff)
{
  ASSERT(buff != NULL);

  return buff->n_wrote == buff->buff_size;
}

bool buff_empty(buff_t *buff)
{
  ASSERT(buff != NULL);

  return buff->n_wrote == 0;
}

size_t buff_write(buff_t *buff, const void *src, size_t size)
{
  ASSERT(buff != NULL);

  if (size > buff->buff_size - buff->n_wrote)
    size = buff->buff_size - buff->n_wrote;

  memcpy(&buff->buff[buff->n_wrote], src, size);
  buff->n_wrote += size;

  return size;
}

size_t buff_read(buff_t *buff, void *dst, size_t size)
{
  ASSERT(buff != NULL);

  if (size > buff->n_wrote)
    size = buff->n_wrote;

  memcpy(dst, &buff->buff[buff->read_ptr], size);
  buff->read_ptr += size;
  buff->n_wrote -= size;

  return size;
}

void buff_reset(buff_t *buff)
{
  ASSERT(buff != NULL);

  buff->read_ptr = 0;
  buff->n_wrote = 0;
}
