#include "longjmp.h"

#include "mem.h"
#include "syscall.h"

task_img_t * syscall_save_ctx(task_img_t *img)
{
  jump_ctx_t *dst = (jump_ctx_t *)img->r1;
  memcpy(&dst->img, img, sizeof(*img));

  return img;
}

task_img_t * syscall_restore_ctx(task_img_t *img)
{
  jump_ctx_t *src = (jump_ctx_t *)img->r1;
  u8 *dst_sp = (u8 *)src->img.sp;

  memcpy(dst_sp, &src->img, sizeof(src->img));

  return (task_img_t *)dst_sp;
}

void longjmp_init()
{
  syscall_register(SYSCALL_SAVE_CTX, syscall_save_ctx);
  syscall_register(SYSCALL_RESTORE_CTX, syscall_restore_ctx);
}

void longjmp(jump_ctx_t *ctx)
{
  ctx->ret = 1;
  syscall(SYSCALL_RESTORE_CTX, ctx);
}



#ifdef TEST

#include "assert.h"
#include "util.h"

bool failed = false;

void f2(jump_ctx_t *ctx)
{
  longjmp(ctx);
  failed = true;
}

void f1(jump_ctx_t *ctx)
{
  f2(ctx);
  failed = true;
}

void f3()
{
}

void longjmp_test()
{
  UT_HEADER();

  jump_ctx_t ctx;

  failed = true;
  if (setjmp(&ctx) == 0) {
    failed = false;
    f1(&ctx);
    failed = true;
  }
  UT_ASSERT(failed == false);

  failed = true;
  if (setjmp(&ctx) == 0) {
    f3();
    failed = false;
  }
  UT_ASSERT(failed == false);

  UT_FOOTER();
}

#endif
