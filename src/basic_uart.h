#pragma once

#include <stdint.h>

void basic_uart_init();
void basic_uart_tx_byte(uint8_t b);
void basic_uart_tx_u8(uint8_t c);
void basic_uart_tx_u16(uint16_t c);
void basic_uart_tx_u32(uint32_t c);
void basic_uart_tx_nl();
