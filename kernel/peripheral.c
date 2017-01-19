#include "peripheral.h"

#include "bits.h"
#include "sam3x8e.h"

void peripheral_enable_clock(peripheral_id_t per)
{
  u32 per_bit = _b(per & 0x1f);

  if (per < 32) {
    Pmc.pcer0 |= per_bit;
    while (bits_clr(Pmc.pcsr0, per_bit)) ;
  }
  else {
    Pmc.pcer1 |= per_bit;
    while (bits_clr(Pmc.pcsr1, per_bit)) ;
  }
}

void peripheral_disable_clock(peripheral_id_t per)
{
  u32 per_bit = _b(per & 0x1f);

  if (per < 32) {
    Pmc.pcdr0 |= per_bit;
    while (bits_set(Pmc.pcsr0, per_bit)) ;
  }
  else {
    Pmc.pcdr1 |= per_bit;
    while (bits_set(Pmc.pcsr1, per_bit)) ;
  }
}

void peripheral_enable_int(peripheral_id_t per)
{
  u32 idx = per / 32;
  u32 bit = per % 32;

  set_bits(Nvic.iser[idx], _b(bit));
}

void peripheral_disable_int(peripheral_id_t per)
{
  u32 idx = per / 32;
  u32 bit = per % 32;

  set_bits(Nvic.icer[idx], _b(bit));
}
