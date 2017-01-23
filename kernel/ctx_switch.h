#pragma once

#include "types.h"

void ctx_switch();
void ctx_exec(u8 *img);
void ctx_save(task_img_t *(*handler)(task_img_t*));

#define DEFINE_CTX_HANDLER(n,f) \
static NAKED void n() \
{ \
  asm ("mov r0, %0\r\n" \
       "b ctx_save\r\n" : : "p" (f)); \
}
