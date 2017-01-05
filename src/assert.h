#pragma once

#include "intrinsic.h"

void abort(const char *reason);

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define ABORT_ACTION(why) abort(why ": " __FILE__ \
                                "[" TOSTRING(__LINE__) "]\r\n"), bkpt()

#define ASSERT(x) do { if (!(x)) ABORT_ACTION("ASSERT failed"); } while (0)
#define ASSERT_RETURN(x, r) \
  do { if (!(x)) { ABORT_ACTION("ASSERT failed"); return r; } } while (0)
#define ASSERT_RETURN_VOID(x) \
  do { if (!(x)) { ABORT_ACTION("ASSERT failed"); return; } } while (0)

#define PANIC(s) { ABORT_ACTION("PANIC " s); }
