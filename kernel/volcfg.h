#pragma once

#include "types.h"

#define VOLCFG_MAGIC 0xdeadbeef

/* Volatile configuration stored in the NFC SRAM area.  Upon first boot the
 * default configuration is copied from flash to the NFC SRAM area. Any
 * configuration changes are retained across resets as well as flash
 * programming until power is removed.
 */
typedef struct _volcfg_t
{
  u32 magic; /* used to detect when to copy from flash */
  bool debug_bkpt; /* break in init() after main clock is enabled */
  bool disable_write_cache; /* disable write cache */
} volcfg_t;

extern volcfg_t VolCfg;

void volcfg_init();
