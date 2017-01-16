#include "basic_uart.h"
#include "bits.h"
#include "clock.h"
#include "idle.h"
#include "intrinsic.h"
#include "nvic.h"
#include "mem.h"
#include "sam3x8e.h"
#include "task.h"
#include "wdt.h"

extern char __data_load__;
extern char __data_start__;
extern char __data_end__;

extern char __bss_start__;
extern char __bss_end__;

NAKED void init()
{
  long data_size = &__data_end__ - &__data_start__;
  long bss_size = &__bss_end__ - &__bss_start__;

  memcpy(&__data_start__, &__data_load__, data_size);
  memzero(&__bss_start__, bss_size);

  init_eefc();
  enable_slow_xtal();
  enable_main_xtal();
  enable_plla();
  switch_to_plla();
  Scb.actlr = _b(1);
  bkpt();

  wdt_disable();

  nvic_init();

  basic_uart_init();
  mem_init();

  sched_init();
  sched_start(idle, 512);
}
