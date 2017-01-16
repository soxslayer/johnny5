#include "main.h"

#include "assert.h"
#include "basic_uart.h"
#include "chardev.h"
#include "peripheral.h"
#include "uart.h"

int main()
{
  peripheral_enable_clock(PIOA_ID);

  basic_uart_tx_str("Johnny 5 alive!!!\r\n");

  fd_t uart = open("usart0");
  ASSERT(uart != INVALID_FD);

  u32 p = 115200;
  ioctl(uart, IOCTL_SETBAUD, &p);

  p = IOCTL_NO_PARITY;
  ioctl(uart, IOCTL_SETPARITY, &p);

  p = IOCTL_1_STOPBIT;
  ioctl(uart, IOCTL_SETSTOP, &p);

  p = IOCTL_LOCAL_LOOPBACK_MODE;
  ioctl(uart, IOCTL_SETCHMODE, &p);

  p = IOCTL_8_BIT_CHARLEN;
  ioctl(uart, IOCTL_SETCHARLEN, &p);

  p = FLAG_BLOCKING;
  ioctl(uart, IOCTL_SETFLAGS, &p);

  u8 b[11];
  while (1) {
    write(uart, "asdfasdf\r\n", 10);
    flush(uart);
    read(uart, b, 10);
    b[10] = 0;
    basic_uart_tx_str((char *)b);
  }

  return 0;
}
