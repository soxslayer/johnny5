#include "main.h"

#include "assert.h"
#include "basic_uart.h"
#include "heap.h"
#include "list.h"
#include "longjmp.h"
#include "mem.h"
#include "syscall.h"

int main()
{
  basic_uart_tx_str("\r\nJohnny 5 Unit Tests\r\n");

  longjmp_test();
  list_test();
  mem_test();
  mem_init();
  heap_test();

  syscall(10, basic_uart_tx_str);

  while (1) {
  }

  return 0;
}
