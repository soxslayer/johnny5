#include "clock.h"

#include "bits.h"
#include "sam3x8e.h"

u64 clock_speed = 12000000;

void init_eefc()
{
  Effc0.fmr = _bm(4, 8);
  Effc1.fmr = _bm(4, 8);
}

void enable_slow_xtal()
{
  if (bits_clr(Supc.sr, _b(3))) {
    Supc.cr = _bm(0xa5, 24) | _b(3);
    while_bits_clr(Supc.sr, _b(7));
  }
}

void enable_main_xtal()
{
  if (bits_clr(Pmc.mor, _b(0))) {
    /* need to keep the RC oscillator enabled while bringing up the xtal */
    Pmc.mor = _bm(0x37, 16) | _bm(0x8, 8) | _b(3) | _b(0);
    while_bits_clr(Pmc.sr, _b(0));

    /* switch source of main clock to xtal */
    Pmc.mor = _bm(0x37, 16) | _bm(0x8, 8) | _b(3) | _b(0) | _b(24);
    while_bits_clr(Pmc.sr, _b(16));

    /* disable the RC oscillator now since it's not needed */
    Pmc.mor = _bm(0x37, 16) | _bm(0x8, 8) | _b(0) | _b(24);
  }
}

void enable_plla()
{
  if (bits_clr(Pmc.sr, _b(1))) {
    Pmc.pllar = _b(29) | _bm(0x6, 16) | _bm(0x8, 8) | 0x1;
    while_bits_clr(Pmc.sr, _b(1));
  }
}

void switch_to_plla()
{
  if (!bits_set(Pmc.mckr, 0x2)) {
    Pmc.mckr = 0x2;
    while_bits_clr(Pmc.sr, _b(3));
  }

  clock_speed = 84000000;
}
