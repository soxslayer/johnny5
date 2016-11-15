#include "clock.h"

#include "bits.h"
#include "reg.h"

void init_eefc()
{
  EEFC0_FMR = FWS(4);
  EEFC1_FMR = FWS(4);
}

void enable_slow_xtal()
{
  if (bits_clr(SUPC_SR, XTALSEL)) {
    SUPC_CR = SUPC_CR_KEY | XTALSEL;
    while_bits_clr(SUPC_SR, OSCSEL);
  }
}

void enable_main_xtal()
{
  if (bits_clr(CKGR_MOR, MOSCXTEN)) {
    /* need to keep the RC oscillator enabled while bringing up the xtal */
    CKGR_MOR = CKGR_MOR_KEY | CKGR_MOR_MOSCXTST(0x8) | MOSCRCEN | MOSCXTEN;
    while_bits_clr(PMC_SR, MOSCXTS);

    /* switch source of main clock to xtal */
    CKGR_MOR = CKGR_MOR_KEY | CKGR_MOR_MOSCXTST(0x8) | MOSCRCEN | MOSCXTEN | MOSCSEL;
    while_bits_clr(PMC_SR, MOSCSELS);

    /* disable the RC oscillator now since it's not needed */
    CKGR_MOR = CKGR_MOR_KEY | CKGR_MOR_MOSCXTST(0x8) | MOSCXTEN | MOSCSEL;
  }
}

void enable_plla()
{
  if (bits_clr(PMC_SR, LOCKA)) {
    CKGR_PLLAR = ONE | MULA(0x6) | DIVA(0x1) | PLLACOUNT(0x8);
    while_bits_clr(PMC_SR, LOCKA);
  }
}

void switch_to_plla()
{
  if (!bits_set(PMC_MCKR, CSS(CSS_PLLA_CLK))) {
    PMC_MCKR = CSS(CSS_PLLA_CLK);
    while_bits_clr(PMC_SR, MCKRDY);
  }
}
