#pragma once

#include "types.h"

void basic_uart_init();
void basic_uart_tx_byte(u8 b);
void basic_uart_tx_u8(u8 c);
void basic_uart_tx_u16(u16 c);
void basic_uart_tx_u32(u32 c);
void basic_uart_tx_str(const char *s);
void basic_uart_tx_nl();
