#include "basic_uart.h"
#include "int_vector.h"

NAKED void main()
{
  basic_uart_init();

  while (1) {
    basic_uart_tx_u32(0x1234abcd);
    basic_uart_tx_nl();
  }
}
