#pragma once

#include <stdint.h>

#include "bits.h"

#define REG32(a) *((volatile uint32_t *)(a))

/* Supply Controller */
#define SUPC_CR REG32(0x400e1a10)
#define VROFF _b(2)
#define XTALSEL _b(3)
#define SUPC_CR_KEY _bm(0xa5, 24)

#define SUPC_SR REG32(0x400e1a24)
#define OSCSEL _b(7)

#define CKGR_MOR REG32(0x400e0620)
#define CFDEN _b(25)
#define MOSCSEL _b(24)
#define CKGR_MOR_KEY _bm(0x37, 16)
#define CKGR_MOR_MOSCXTST(b) _bm(0xff & (b), 8)
#define MOSCRCEN _b(3)
#define MOSCXTBY _b(1)
#define MOSCXTEN _b(0)

#define PMC_SR REG32(0x400e0668)
#define FOS _b(20)
#define CFDS _b(19)
#define CFDEV _b(18)
#define MOSCRCS _b(17)
#define MOSCSELS _b(16)
#define PCKRDY2 _b(10)
#define PCKRDY1 _b(9)
#define PCKRDY0 _b(8)
#define OSCSELS _b(7)
#define LOCKU _b(6)
#define MCKRDY _b(3)
#define LOCKA _b(1)
#define MOSCXTS _b(0)

#define PMC_MCKR REG32(0x400e0630)
#define UPLLDIV2 _b(13)
#define PLLADIV2 _b(12)
#define PRES(x) _bm((x) & 0x7, 4)
# define PRES_CLK 0x0
# define PRES_CLK_2 0x1
# define PRES_CLK_4 0x2
# define PRES_CLK_8 0x3
# define PRES_CLK_16 0x4
# define PRES_CLK_32 0x5
# define PRES_CLK_64 0x6
# define PRES_CLK_3 0x7
#define CSS(x) ((x) & 3)
# define CSS_SLOW_CLK 0x0
# define CSS_MAIN_CLK 0x1
# define CSS_PLLA_CLK 0x2
# define CSS_UPLL_CLK 0x3

#define CKGR_PLLAR REG32(0x400e0628)
#define ONE _b(29)
#define MULA(x) _bm((x) & 0x7ff, 16)
#define PLLACOUNT(x) _bm((x) & 0x3f, 8)
#define DIVA(x) ((x) & 0xff)

#define EEFC0_FMR REG32(0x400e0a00)
#define EEFC1_FMR REG32(0x400e0c00)
# define FAM _b(24)
# define FWS(x) _bm((x) & 0xf, 8)
# define FRDY _b(0)
