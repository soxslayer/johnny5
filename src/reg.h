#pragma once

#include <stdint.h>

#include "bits.h"

#define REG32(a) *((volatile uint32_t *)(a))

/* Supply Controller */
#define SUPC_CR REG32(0x400e1a10)
# define VROFF _b(2)
# define XTALSEL _b(3)
# define SUPC_CR_KEY _bm(0xa5, 24)
#define SUPC_SR REG32(0x400e1a24)
# define OSCSEL _b(7)

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
# define ONE _b(29)
# define MULA(x) _bm((x) & 0x7ff, 16)
# define PLLACOUNT(x) _bm((x) & 0x3f, 8)
# define DIVA(x) ((x) & 0xff)

#define PMC_PCER0 REG32(0x400e0610)
#define PMC_PCDR0 REG32(0x400e0614)
#define PMC_PCSR0 REG32(0x400e0618)
#define PMC_PCER1 REG32(0x400e0700)
#define PMC_PCDR1 REG32(0x400e0704)
#define PMC_PCSR1 REG32(0x400e0708)
# define PID2 _b(2)
# define PID3 _b(3)
# define PID4 _b(4)
# define PID5 _b(5)
# define PID6 _b(6)
# define PID7 _b(7)
# define PID8 _b(8)
# define PID9 _b(9)
# define PID10 _b(10)
# define PID11 _b(11)
# define PID12 _b(12)
# define PID13 _b(13)
# define PID14 _b(14)
# define PID15 _b(15)
# define PID16 _b(16)
# define PID17 _b(17)
# define PID18 _b(18)
# define PID19 _b(19)
# define PID20 _b(20)
# define PID21 _b(21)
# define PID22 _b(22)
# define PID23 _b(23)
# define PID24 _b(24)
# define PID25 _b(25)
# define PID26 _b(26)
# define PID27 _b(27)
# define PID28 _b(28)
# define PID29 _b(29)
# define PID30 _b(30)
# define PID31 _b(31)
# define PID32 _b(32)
# define PID33 _b(33)
# define PID34 _b(34)
# define PID35 _b(35)
# define PID36 _b(36)
# define PID37 _b(37)
# define PID38 _b(38)
# define PID39 _b(39)
# define PID40 _b(40)
# define PID41 _b(41)
# define PID42 _b(42)
# define PID43 _b(43)
# define PID44 _b(44)

#define EEFC0_FMR REG32(0x400e0a00)
#define EEFC1_FMR REG32(0x400e0c00)
# define FAM _b(24)
# define FWS(x) _bm((x) & 0xf, 8)
# define FRDY _b(0)

/* UART */
#define UART_CR REG32(0x400e0800)
# define RSTRX _b(2)
# define RSTTX _b(3)
# define RXEN _b(4)
# define RXDIS _b(5)
# define TXEN _b(6)
# define TXDIS _b(7)
# define RSTSTA _b(8)
#define UART_MR REG32(0x400e0804)
# define PAR(x) _bm((x) & 7, 9)
#  define PAR_EVEN 0x0
#  define PAR_ODD 0x1
#  define PAR_SPACE 0x2
#  define PAR_MARK 0x3
#  define PAR_NO 0x4
# define CHMODE(x) _bm((x) & 3, 14)
#  define CHMODE_NORMAL 0x0
#  define CHMODE_AUTOMATIC 0x1
#  define CHMODE_LOCAL_LOOPBACK 0x2
#  define CHMODE_REMOTE_LOOPBACK 0x3
#define UART_SR REG32(0x400e0814)
# define RXRDY _b(0)
# define TXRDY _b(1)
# define ENDRX _b(3)
# define ENDTX _b(4)
# define OVRE _b(5)
# define FRAME _b(6)
# define PARE _b(7)
# define TXEMPTY _b(9)
# define TXBUFE _b(11)
# define RXBUFF _b(12)
#define UART_BRGR REG32(0x400e0820)
# define CD(x) ((x) & 0xffff)
#define UART_THR REG32(0x400e081c)
# define TXCHR(x) ((x) & 0xff)

#define WDT_CR REG32(0x400e1a50)
# define WDT_KEY _bm(0xa5, 24)
# define WDRSTT _b(0)
#define WDT_MR REG32(0x400e1a54)
# define WDV(x) ((x) & 0xfff)
# define WDFIEN _b(12)
# define WDRSTEN _b(13)
# define WDRPROC _b(14)
# define WDDIS _b(15)
# define WDD(x) _bm((x) & 0xfff, 16)
# define WDDBGHLT _b(28)
# define WDIDLEHLT _b(29)

#define PIO_PDRA REG32(0x400e0e04)
# define P9 _b(9)
