#include "assert.h"

#include "basic_uart.h"
#include "nvic.h"

void abort(const char *reason)
{
  nvic_disable_int();

  basic_uart_tx_str(reason);
}
