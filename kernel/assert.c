#include "assert.h"

#include "basic_uart.h"

#ifdef TEST
jump_ctx_t __err_jump;
bool __err_jump_valid = false;
#endif

void abort(const char *reason)
{
  basic_uart_tx_str(reason);
}
