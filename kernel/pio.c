#include "pio.h"

#include "assert.h"
#include "mem.h"

void pio_enable(pio_regs_t *pio, pio_pin_t pin)
{
  ASSERT(pio != NULL);

  pio->per = _b(pin);
}

void pio_disable(pio_regs_t *pio, pio_pin_t pin)
{
  ASSERT(pio != NULL);

  pio->pdr = _b(pin);
}

void pio_enable_output(pio_regs_t *pio, pio_pin_t pin)
{
  ASSERT(pio != NULL);

  pio->oer = _b(pin);
}

void pio_disable_output(pio_regs_t *pio, pio_pin_t pin)
{
  ASSERT(pio != NULL);

  pio->odr = _b(pin);
}

void pio_set_peripheral_mode(pio_regs_t *pio, pio_pin_t pin, pio_mode_t mode)
{
  ASSERT(pio != NULL);

  if (mode == PIO_MODEA)
    clr_bits(pio->absr, _b(pin));
  else
    set_bits(pio->absr, _b(pin));
}

void pio_enable_pullup(pio_regs_t *pio, pio_pin_t pin)
{
  ASSERT(pio != NULL);

  pio->puer = _b(pin);
}

void pio_disable_pullup(pio_regs_t *pio, pio_pin_t pin)
{
  ASSERT(pio != NULL);

  pio->pudr = _b(pin);
}

void pio_set_pin(pio_regs_t *pio, pio_pin_t pin, u8 val)
{
  ASSERT(pio != NULL);

  if (val)
    pio_assert_pin(pio, pin);
  else
    pio_deassert_pin(pio, pin);
}

void pio_assert_pin(pio_regs_t *pio, pio_pin_t pin)
{
  ASSERT(pio != NULL);

  pio->sodr = _b(pin);
}

void pio_deassert_pin(pio_regs_t *pio, pio_pin_t pin)
{
  ASSERT(pio != NULL);

  pio->codr = _b(pin);
}

void pio_toggle_pin(pio_regs_t *pio, pio_pin_t pin)
{
  ASSERT(pio != NULL);

  if (bits_set(pio->odsr, _b(pin)))
    pio_deassert_pin(pio, pin);
  else
    pio_assert_pin(pio, pin);
}
