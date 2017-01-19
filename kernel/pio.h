#pragma once

#include "sam3x8e.h"

typedef enum {
  PIN0,
  PIN1,
  PIN2,
  PIN3,
  PIN4,
  PIN5,
  PIN6,
  PIN7,
  PIN8,
  PIN9,
  PIN10,
  PIN11,
  PIN12,
  PIN13,
  PIN14,
  PIN15,
  PIN16,
  PIN17,
  PIN18,
  PIN19,
  PIN20,
  PIN21,
  PIN22,
  PIN23,
  PIN24,
  PIN25,
  PIN26,
  PIN27,
  PIN28,
  PIN29,
  PIN30,
  PIN31
} pio_pin_t;

typedef enum {
  PIO_MODEA,
  PIO_MODEB
} pio_mode_t;

void pio_enable(pio_regs_t *pio, pio_pin_t pin);
void pio_disable(pio_regs_t *pio, pio_pin_t pin);
void pio_enable_output(pio_regs_t *pio, pio_pin_t pin);
void pio_disable_output(pio_regs_t *pio, pio_pin_t pin);
void pio_set_peripheral_mode(pio_regs_t *pio, pio_pin_t pin, pio_mode_t mode);
void pio_enable_pullup(pio_regs_t *pio, pio_pin_t pin);
void pio_disable_pullup(pio_regs_t *pio, pio_pin_t pin);
void pio_set_pin(pio_regs_t *pio, pio_pin_t pin, u8 val);
void pio_assert_pin(pio_regs_t *pio, pio_pin_t pin);
void pio_deassert_pin(pio_regs_t *pio, pio_pin_t pin);
void pio_toggle_pin(pio_regs_t *pio, pio_pin_t pin);
