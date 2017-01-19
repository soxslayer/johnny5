#include "basic_uart.h"
#include "bits.h"
#include "clock.h"
#include "idle.h"
#include "intrinsic.h"
#include "longjmp.h"
#include "mem.h"
#include "nvic.h"
#include "sam3x8e.h"
#include "syscall.h"
#include "task.h"
#include "volcfg.h"
#include "wdt.h"

#ifdef TEST
# include "main.h"
#endif

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

  volcfg_init();

  if (VolCfg.disable_write_cache)
    Scb.actlr = _b(1);
  if (VolCfg.debug_bkpt)
    bkpt();

  wdt_disable();

  nvic_init();
  syscall_init();
  longjmp_init();

  basic_uart_init();

#ifdef TEST
  main();
#else
  mem_init();
  sched_init();
  sched_start(idle, 512);
#endif
}
