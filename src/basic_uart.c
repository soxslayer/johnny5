#include "basic_uart.h"

#include "bits.h"
#include "clock.h"
#include "sam3x8e.h"

static u8 nibble_to_ascii(u8 n)
{
  u8 nib = n & 0xf;

  if (nib < 0xa)
    return nib + '0';

  return nib - 10 + 'a';
}

void basic_uart_init()
{
  Pmc.pcer0 = _b(8);
  /* 115200 baud */
  Uart0.brgr = (clock_speed / 1843200) & 0xffff;
  Uart0.mr = _bm(0x4, 9);
  Uart0.cr = _b(6);
  PioA.pdr = _b(9);
}

void basic_uart_tx_byte(u8 b)
{
  while_bits_clr(Uart0.sr, _b(1));
  Uart0.thr = b;
}

void basic_uart_tx_u8(u8 c)
{
  basic_uart_tx_byte('0');
  basic_uart_tx_byte('x');
  basic_uart_tx_byte(nibble_to_ascii(c >> 4));
  basic_uart_tx_byte(nibble_to_ascii(c));
}

void basic_uart_tx_u16(u16 c)
{
  basic_uart_tx_byte('0');
  basic_uart_tx_byte('x');
  basic_uart_tx_byte(nibble_to_ascii(c >> 12));
  basic_uart_tx_byte(nibble_to_ascii(c >> 8));
  basic_uart_tx_byte(nibble_to_ascii(c >> 4));
  basic_uart_tx_byte(nibble_to_ascii(c));
}

void basic_uart_tx_u32(u32 c)
{
  basic_uart_tx_byte('0');
  basic_uart_tx_byte('x');
  basic_uart_tx_byte(nibble_to_ascii(c >> 28));
  basic_uart_tx_byte(nibble_to_ascii(c >> 24));
  basic_uart_tx_byte(nibble_to_ascii(c >> 20));
  basic_uart_tx_byte(nibble_to_ascii(c >> 16));
  basic_uart_tx_byte(nibble_to_ascii(c >> 12));
  basic_uart_tx_byte(nibble_to_ascii(c >> 8));
  basic_uart_tx_byte(nibble_to_ascii(c >> 4));
  basic_uart_tx_byte(nibble_to_ascii(c));
}

void basic_uart_tx_str(const char *s)
{
  for (int i = 0; s[i] != 0; ++i)
    basic_uart_tx_byte(s[i]);
}

void basic_uart_tx_nl()
{
  basic_uart_tx_byte('\r');
  basic_uart_tx_byte('\n');
}
