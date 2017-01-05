#include "main.h"

#include "basic_uart.h"

int main()
{
  basic_uart_tx_str("Johnny 5 alive!!!\r\n");

  while (1) {
  }

  return 0;
}
