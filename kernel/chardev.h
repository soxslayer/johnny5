#pragma once

#include "bits.h"
#include "handle_array.h"
#include "hash_table.h"
#include "limits.h"
#include "signal.h"
#include "spinlock.h"
#include "types.h"

#define CHARDEV_NAME_MAX_LENGTH 16

typedef struct _chardev_t
{
  int (*open)(struct _chardev_t *dev);
  int (*close)(struct _chardev_t *dev);
  ssize_t (*read)(struct _chardev_t *dev, void *dst, size_t size);
  ssize_t (*write)(struct _chardev_t *dev, const void *src, size_t size);
  size_t (*poll)(struct _chardev_t *dev);
  int (*flush)(struct _chardev_t *dev);
  int (*ioctl)(struct _chardev_t *dev, u32 ioop, void *param);
  hash_table_node_t node;
  char name[CHARDEV_NAME_MAX_LENGTH];
  u32 flags;
  spinlock_t lock;
  signal_t wait_list;
} chardev_t;

void chardev_init();
void chardev_register(chardev_t *dev);
void chardev_unregister(chardev_t *dev);

typedef u32 fd_t;

#define INVALID_FD INVALID_HANDLE
#define CHARDEV_ERROR -1

/* generic ioctls */
#define IOCTL_SETFLAGS 0
#define IOCTL_CLRFLAGS 1

/* uart ioctls */
#define IOCTL_SETBAUD 2
#define IOCTL_SETPARITY 3
#define IOCTL_SETSTOP 4
#define IOCTL_SETCHMODE 5
#define IOCTL_SETCHARLEN 6

#define FLAG_BLOCKING _b(0)

fd_t open(const char *name);
void close(fd_t fd);
ssize_t read(fd_t fd, void *dst, size_t size);
ssize_t write(fd_t fd, const void *src, size_t size);
size_t poll(fd_t fd);
int flush(fd_t fd);
int ioctl(fd_t fd, u32 ioop, void *param);
