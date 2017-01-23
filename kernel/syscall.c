#include "syscall.h"

#include "ctx_switch.h"
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

DEFINE_CTX_HANDLER(svc_exception_entry, syscall_handle);

void syscall_init()
{
  nvic_set_handler(NVIC_SVCALL, svc_exception_entry);
}

void syscall_register(syscall_id_t num, syscall_t handler)
{
  if (num < SYSCALL_MAX)
    __syscalls[num] = handler;
}

void NAKED syscall(syscall_id_t num, void *data)
{
  if (num == SYSCALL_MAX)
    asm ("bx lr");

  asm ("mov r0, %0\r\n"
       "mov r1, %1\r\n"
       "svc 0\r\n"
       "bx lr\r\n" : : "r" (num), "r" (data));
}
