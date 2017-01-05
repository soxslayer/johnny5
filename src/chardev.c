#include "chardev.h"

#include "assert.h"
#include "bits.h"
#include "hash_table.h"
#include "sem.h"
#include "string.h"

#define MAX_FDS 32

sem_t __devs_sem = BINARY_SEM_INIT;
hash_table_t __devs;

sem_t __fds_sem = BINARY_SEM_INIT;
handle_array_t __fds;

static void get_chardev_key(hash_table_node_t *node, void **key, size_t *size)
{
  chardev_t *dev = containerof(node, chardev_t, node);
  *key = dev->name;
  *size = strnlen(dev->name, CHARDEV_NAME_MAX_LENGTH);
}

void chardev_init()
{
  hash_table_create(&__devs, 8, get_chardev_key);
  handle_array_create(&__fds, MAX_FDS);
}

void chardev_register(chardev_t *dev)
{
  ASSERT(dev != NULL);

  sem_take(&__devs_sem);

  hash_table_insert(&__devs, &dev->node);

  sem_give(&__devs_sem);
}

void chardev_unregister(chardev_t *dev)
{
  ASSERT(dev != NULL);

  sem_take(&__devs_sem);

  hash_table_remove(&__devs, &dev->node);

  sem_give(&__devs_sem);
}



static chardev_t * get_dev(fd_t fd)
{
  sem_take(&__fds_sem);

  chardev_t *dev = handle_array_get(&__fds, fd);

  sem_give(&__fds_sem);

  return dev;
}

fd_t open(const char *name)
{
  sem_take(&__devs_sem);

  chardev_t *dev = containerof(hash_table_get(&__devs, name,
                                 strnlen(name, CHARDEV_NAME_MAX_LENGTH)),
                               chardev_t, node);

  int rc = dev->open(dev);

  sem_give(&__devs_sem);

  if (rc == CHARDEV_ERROR)
    return INVALID_FD;

  sem_take(&__fds_sem);

  fd_t fd = handle_array_alloc(&__fds, dev);

  sem_give(&__fds_sem);

  if (fd == INVALID_FD) {
    dev->close(dev);
    return INVALID_FD;
  }

  dev->flags = 0;

  return fd;
}

void close(fd_t fd)
{
  ASSERT(fd < MAX_FDS);

  chardev_t *dev = get_dev(fd);

  dev->close(dev);

  sem_take(&__fds_sem);

  handle_array_free(&__fds, fd);

  sem_give(&__fds_sem);
}

ssize_t read(fd_t fd, void *dst, size_t size)
{
  ASSERT(fd < MAX_FDS);

  chardev_t *dev = get_dev(fd);

  ssize_t nread = 0;

  do {
    ssize_t nr = dev->read(dev, ((u8 *)dst) + nread, size - nread);
    if (nr == CHARDEV_ERROR)
      return CHARDEV_ERROR;

    nread += nr;
  } while (bits_clr(dev->flags, _bm(1, FLAG_NONBLOCKING)) && nread < size);

  return nread;
}

ssize_t write(fd_t fd, const void *src, size_t size)
{
  ASSERT(fd < MAX_FDS);

  chardev_t *dev = get_dev(fd);

  ssize_t nwrite = 0;

  do {
    ssize_t nw = dev->write(dev, ((u8 *)src) + nwrite, size - nwrite);
    if (nw == CHARDEV_ERROR)
      return CHARDEV_ERROR;

    nwrite += nw;
  } while (bits_clr(dev->flags, _bm(1, FLAG_NONBLOCKING)) && nwrite < size);

  return nwrite;
}
