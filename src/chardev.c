#include "chardev.h"

#include "assert.h"
#include "bits.h"
#include "hash_table.h"
#include "sem.h"
#include "string.h"

#define MAX_FDS 32

spinlock_t __devs_lock;
hash_table_t __devs;

spinlock_t __fds_sem;
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
  spinlock_init(&__devs_lock);
  spinlock_init(&__fds_sem);
}

void chardev_register(chardev_t *dev)
{
  ASSERT(dev != NULL);

  dev->flags = 0;
  spinlock_init(&dev->lock);

  spinlock_acquire(&__devs_lock);

  hash_table_insert(&__devs, &dev->node);

  spinlock_release(&__devs_lock);
}

void chardev_unregister(chardev_t *dev)
{
  ASSERT(dev != NULL);

  spinlock_acquire(&__devs_lock);

  hash_table_remove(&__devs, &dev->node);

  spinlock_release(&__devs_lock);
}



static chardev_t * get_dev(fd_t fd)
{
  spinlock_acquire(&__fds_sem);

  chardev_t *dev = handle_array_get(&__fds, fd);

  spinlock_release(&__fds_sem);

  return dev;
}

fd_t open(const char *name)
{
  spinlock_acquire(&__devs_lock);

  chardev_t *dev = containerof(hash_table_get(&__devs, name,
                                 strnlen(name, CHARDEV_NAME_MAX_LENGTH)),
                               chardev_t, node);

  ASSERT(dev != NULL);

  int rc = dev->open(dev);

  spinlock_release(&__devs_lock);

  if (rc == CHARDEV_ERROR)
    return INVALID_FD;

  spinlock_acquire(&__fds_sem);

  fd_t fd = handle_array_alloc(&__fds, dev);

  spinlock_release(&__fds_sem);

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

  ASSERT(dev != NULL);

  dev->close(dev);

  spinlock_acquire(&__fds_sem);

  handle_array_free(&__fds, fd);

  spinlock_release(&__fds_sem);
}

ssize_t read(fd_t fd, void *dst, size_t size)
{
  ASSERT(fd < MAX_FDS);

  chardev_t *dev = get_dev(fd);

  ASSERT(dev != NULL && dst != NULL);

  ssize_t nread = 0;

  do {
    ssize_t nr = dev->read(dev, ((u8 *)dst) + nread, size - nread);
    if (nr == CHARDEV_ERROR)
      return CHARDEV_ERROR;

    nread += nr;
  } while (bits_set(dev->flags, FLAG_BLOCKING) && nread < size);

  return nread;
}

ssize_t write(fd_t fd, const void *src, size_t size)
{
  ASSERT(fd < MAX_FDS);

  chardev_t *dev = get_dev(fd);

  ASSERT(dev != NULL && src != NULL);

  ssize_t nwrite = 0;

  do {
    ssize_t nw = dev->write(dev, ((u8 *)src) + nwrite, size - nwrite);
    if (nw == CHARDEV_ERROR)
      return CHARDEV_ERROR;

    nwrite += nw;
  } while (bits_set(dev->flags, FLAG_BLOCKING) && nwrite < size);

  return nwrite;
}

size_t poll(fd_t fd)
{
  ASSERT(fd < MAX_FDS);

  chardev_t *dev = get_dev(fd);

  ASSERT(dev != NULL);

  return dev->poll(dev);
}

int flush(fd_t fd)
{
  ASSERT(fd < MAX_FDS);

  chardev_t *dev = get_dev(fd);

  ASSERT(dev);

  return dev->flush(dev);
}

int ioctl(fd_t fd, u32 ioop, void *param)
{
  ASSERT(fd < MAX_FDS);

  chardev_t *dev = get_dev(fd);

  ASSERT(dev != NULL);

  switch (ioop) {
    case IOCTL_SETFLAGS: {
      ASSERT(param != NULL);

      spinlock_acquire(&dev->lock);

      int r = set_bits(dev->flags, *((u32 *)param));

      spinlock_release(&dev->lock);

      return r;
    }

    case IOCTL_CLRFLAGS: {
      ASSERT(param != NULL);

      spinlock_acquire(&dev->lock);

      int r = clr_bits(dev->flags, *((u32 *)param));

      spinlock_release(&dev->lock);

      return r;
    }

    default:
      return dev->ioctl(dev, ioop, param);
  }

  return -1;
}
