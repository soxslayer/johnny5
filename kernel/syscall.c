#include "syscall.h"

#include "nvic.h"
#include "mem.h"

static syscall_t __syscalls[SYSCALL_MAX] ALIGNED(4) = {
  [0 ... SYSCALL_MAX - 1] = SYSCALL_DEFAULT_HANDLER
};

static task_img_t * syscall_handle(task_img_t *img)
{
  syscall_id_t id = (syscall_id_t)img->r0;

  if (id < SYSCALL_MAX && __syscalls[id] != NULL)
    return __syscalls[id](img);

  return img;
}

static NAKED void svc_exception_entry()
{
  asm ("ands r1, lr, #4\r\n"
       "itte eq\r\n"
       "mrseq r0, msp\r\n"
       "subeq sp, sp, #36\r\n"
       "mrsne r0, psp\r\n"
       "mov r2, r0\r\n"
       "sub r2, r2, #36\r\n"
       "stmdb r0!, {r2,r4-r11}\r\n"
       "push {r1,lr}\r\n"
       "bl syscall_handle\r\n"
       "pop {r1,lr}\r\n"
       "ldm r0, {r2,r4-r11}\r\n"
       "add r2, r2, #36\r\n"
       "cmp r1, #0\r\n"
       "ite eq\r\n"
       "msreq msp, r2\r\n"
       "msrne psp, r2\r\n"
       "bx lr");
}

void syscall_init()
{
  nvic_set_handler(NVIC_SVCALL, svc_exception_entry);
}

void syscall_register(syscall_id_t num, syscall_t handler)
{
  if (num < SYSCALL_MAX)
    __syscalls[num] = handler;
}

NAKED void syscall(syscall_id_t num, void *data)
{
  if (num == SYSCALL_MAX)
    return;

  asm ("svc 0\r\n"
       "bx lr\r\n");
}
