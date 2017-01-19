#pragma once

#include "intrinsic.h"
#include "nvic.h"
#include "util.h"

void abort(const char *reason);

#define ABORT_ACTION(why) abort(why ": " __FILE__ \
                                "[" TOSTRING(__LINE__) "]\r\n")

#ifdef TEST
# include "longjmp.h"
#include "basic_uart.h"

extern jump_ctx_t __err_jump;
extern bool __err_jump_valid;

# define ASSERT(x) \
({ \
  if (!(x)) { \
    if (__err_jump_valid) { \
      ABORT_ACTION("expected failure"); \
      longjmp(&__err_jump); \
    } \
    else { \
      ABORT_ACTION("ASSERT failed"); \
      bkpt(); \
    } \
  } \
})
# define PANIC(s) \
({ \
  if (__err_jump_valid) { \
    ABORT_ACTION("expected panic"); \
    longjmp(&__err_jump); \
  } \
  else { \
    ABORT_ACTION("PANIC " s); \
    bkpt(); \
  } \
})

# define UT_ASSERT(x) ({ if (!(x)) { ABORT_ACTION("ASSERT failed"); bkpt(); } })
# define UT_ASSERT_FAIL(x) \
({ \
  if (({ __err_jump_valid = true; setjmp(&__err_jump); }) == 0) { \
    x; \
    ABORT_ACTION("did not received expected failure"); \
    bkpt(); \
  } \
  __err_jump_valid = false; \
})
# define UT_ASSERT_PASS(x) \
({ \
  if (({ __err_jump_avlid = true; setjmp(&__err_jump); }) == 0) { \
    x; \
  } \
  else { \
    ABORT_ACTION("unexpected failure"); \
    bkpt(); \
  } \
  __err_jump_alid = false; \
})
#else
# define ASSERT(x) \
  ({ if (!(x)) { ABORT_ACTION("ASSERT failed"); bkpt(); } })
# define PANIC(s) ({ ABORT_ACTION("PANIC " s); bkpt(); })
#endif
