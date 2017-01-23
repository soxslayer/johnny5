#include "longjmp.h"

#include "mem.h"
#include "syscall.h"

void longjmp(jump_ctx_t *ctx)
{
  ctx->ret = 1;
  syscall(SYSCALL_RESTORE_CTX, &ctx->img);
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
