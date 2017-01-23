#pragma once

#include "syscall.h"
#include "task.h"

typedef struct _jump_ctx_t
{
  task_img_t img;
  int ret;
} jump_ctx_t;

void longjmp_init();
#define setjmp(c) \
({ \
  (c)->ret = 0; \
  syscall(SYSCALL_SAVE_CTX, &(c)->img); \
  (c)->ret; \
})
void longjmp(jump_ctx_t *ctx);

#ifdef TEST
void longjmp_test();
#endif
