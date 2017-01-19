#pragma once

#include "mem.h"
#include "task.h"
#include "types.h"
#include "util.h"

typedef task_img_t * (*syscall_t)(task_img_t *);

typedef enum {
  SYSCALL_TASK_YIELD,
  SYSCALL_SAVE_CTX,
  SYSCALL_RESTORE_CTX,
  SYSCALL_MAX /* must be last */
} syscall_id_t;

#define SYSCALL_DEFAULT_HANDLER NULL

void syscall_init();
void syscall_register(syscall_id_t num, syscall_t handler);
void syscall(syscall_id_t num, void *data);
