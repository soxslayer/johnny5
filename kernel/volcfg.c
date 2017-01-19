#include "volcfg.h"

#include "mem.h"
#include "peripheral.h"

volcfg_t VolCfg SECTION(".volcfg") = { 
  .magic = VOLCFG_MAGIC,
  .debug_bkpt = false,
  .disable_write_cache = false
};

extern u8 __volcfg_load__;
extern u8 __volcfg_start__;
extern u8 __volcfg_end__;

void volcfg_init()
{
  peripheral_enable_clock(SMC_SDRAMC_ID);

  if (VolCfg.magic != VOLCFG_MAGIC) {
    long size = &__volcfg_end__ - &__volcfg_start__;
    memcpy(&__volcfg_start__, &__volcfg_load__, size);
  }
}
