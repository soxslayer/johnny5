#include "basic_uart.h"

#include "bits.h"
#include "reg.h"

static uint8_t nibble_to_ascii(uint8_t n)
{
  uint8_t nib = n & 0xf;

  if (nib < 0xa)
    return nib + '0';

  return nib - 10 + 'a';
}

void basic_uart_init()
{
  PMC_PCER0 = PID8;
  /* 115200 baud */
  UART_BRGR = CD(0x2e);
  UART_MR = PAR(PAR_NO);
  UART_CR = TXEN;
  PIO_PDRA = P9;
}

void basic_uart_tx_byte(uint8_t b)
{
  while_bits_clr(UART_SR, TXRDY);
  UART_THR = TXCHR(b);
}

void basic_uart_tx_u8(uint8_t c)
{
  basic_uart_tx_byte('0');
  basic_uart_tx_byte('x');
  basic_uart_tx_byte(nibble_to_ascii(c >> 4));
  basic_uart_tx_byte(nibble_to_ascii(c));
}

void basic_uart_tx_u16(uint16_t c)
{
  basic_uart_tx_byte('0');
  basic_uart_tx_byte('x');
  basic_uart_tx_byte(nibble_to_ascii(c >> 12));
  basic_uart_tx_byte(nibble_to_ascii(c >> 8));
  basic_uart_tx_byte(nibble_to_ascii(c >> 4));
  basic_uart_tx_byte(nibble_to_ascii(c));
}

void basic_uart_tx_u32(uint32_t c)
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

void basic_uart_tx_nl()
{
  basic_uart_tx_byte('\r');
  basic_uart_tx_byte('\n');
}
