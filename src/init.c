#include <stdint.h>

#include "bits.h"
#include "int_vector.h"

#include "clock.h"
#include "reg.h"

extern char * __data_load__;
extern char *__data_start__;
extern char *__data_end__;

extern char *__bss_start__;
extern char *__bss_end__;

extern NAKED void main();

NAKED void init()
{
  long data_size = __data_end__ - __data_start__;
  long bss_size = __bss_end__ - __bss_start__;

  /* copy .data section from flash to sram */
  for (long i = 0; i < data_size; ++i)
    __data_start__[i] = __data_load__[i];

  /* zero out .bss section */
  for (long i = 0; i < bss_size; ++i)
    __bss_start__[i] = 0;

  init_eefc();
  enable_slow_xtal();
  enable_main_xtal();
  enable_plla();
  switch_to_plla();

  main();
}
