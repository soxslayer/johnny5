#pragma once

#include "bits.h"
#include "types.h"

typedef volatile struct {
  void * rpr;
  u32 rcr;
  void * tpr;
  u32 tcr;
  void * rnpr;
  u32 rncr;
  void * tnpr;
  u32 tncr;
  u32 ptcr;
  u32 ptsr;
} pdc_regs_t;

typedef volatile struct {
  u32 cr;
  u32 mr;
  u32 ier;
  u32 idr;
  const u32 imr;
  const u32 sr;
  const u32 rhr;
  u32 thr;
  u32 brgr;
  /* not present for Uart0 */
  u32 rtor;
  u32 ttgr;
  const u32 reserved1[5];
  u32 fidi;
  const u32 ner;
  const u32 reserved2;
  u32 _if;
  u32 man;
  u32 linmr;
  u32 linir;
  const u32 reserved3[34];
  u32 wpmr;
  const u32 wpsr;
  const u32 reserved4[4];
  const u32 version;
  pdc_regs_t pdc;
} uart_regs_t;

extern uart_regs_t Uart0;
extern uart_regs_t Usart0;
extern uart_regs_t Usart1;
extern uart_regs_t Usart2;
extern uart_regs_t Usart3;

typedef volatile struct {
  u32 scer;
  u32 scdr;
  const u32 scsr;
  const u32 reserved1;
  u32 pcer0;
  u32 pcdr0;
  const u32 pcsr0;
  u32 uckr;
  u32 mor;
  const u32 mcfr;
  u32 pllar;
  const u32 reserved2;
  u32 mckr;
  const u32 reserved3;
  u32 usb;
  const u32 reserved4;
  u32 pck0;
  u32 pck1;
  u32 pck2;
  const u32 reserved5[5];
  u32 ier;
  u32 idr;
  const u32 sr;
  const u32 imr;
  u32 fsmr;
  u32 fspr;
  u32 focr;
  const u32 reserved6[26];
  u32 wpmr;
  const u32 wpsr;
  const u32 reserved7[5];
  u32 pcer1;
  u32 pcdr1;
  const u32 pcsr1;
  u32 pcr;
} pmc_regs_t;

extern pmc_regs_t Pmc;

typedef volatile struct {
  u32 per;
  u32 pdr;
  const u32 psr;
  const u32 reserved1;
  u32 oer;
  u32 odr;
  const u32 osr;
  const u32 reserved2;
  u32 ifer;
  u32 ifdr;
  const u32 ifsr;
  const u32 reserved3;
  u32 sodr;
  u32 codr;
  u32 odsr;
  const u32 pdsr;
  u32 ier;
  u32 idr;
  const u32 imr;
  const u32 isr;
  u32 mder;
  u32 mddr;
  const u32 mdsr;
  const u32 reserved4;
  u32 pudr;
  u32 puer;
  const u32 pusr;
  const u32 reserved5;
  u32 absr;
  const u32 reserved6[3];
  u32 scifsr;
  u32 difsr;
  const u32 ifdgsr;
  u32 scdr;
  const u32 reserved7[4];
  u32 ower;
  u32 owdr;
  const u32 owsr;
  const u32 reserved8;
  u32 aimer;
  u32 aimdr;
  const u32 aimmr;
  const u32 reserved9;
  u32 esr;
  u32 lsr;
  const u32 elsr;
  const u32 reserved10;
  u32 fellsr;
  u32 rehlsr;
  const u32 frlhsr;
  const u32 reserved11;
  const u32 locksr;
  u32 wpmr;
  u32 wpsr;
} pio_regs_t;

extern pio_regs_t PioA;
extern pio_regs_t PioB;
extern pio_regs_t PioC;
extern pio_regs_t PioD;

typedef volatile struct {
  u32 fmr;
  u32 fcr;
  const u32 fsr;
  const u32 frr;
} eefc_regs_t;

extern eefc_regs_t Effc0;
extern eefc_regs_t Effc1;

typedef volatile struct {
  const u32 reserved1[2];
  u32 actlr;
  const u32 reserved2[829];
  const u32 cpuid;
  u32 icsr;
  u32 vtor;
  u32 aircr;
  u32 scr;
  u32 ccr;
  u32 shpr1;
  u32 shpr2;
  u32 shpr3;
  u32 shcrs;
  u8 mmsr;
  u8 bfsr;
  u16 ufsr;
  u32 hfsr;
  u32 reserved3;
  u32 mmar;
  u32 bfar;
} scb_regs_t;

extern scb_regs_t Scb;

typedef volatile struct {
  u32 ctrl;
  u32 load;
  u32 val;
  const u32 calib;
} systick_regs_t;

extern systick_regs_t SysTick;

typedef volatile struct {
  u32 cr;
  const u32 sr;
  u32 mr;
} rstc_regs_t;

extern rstc_regs_t Rstc;

typedef volatile struct {
  u32 cr;
  u32 smmr;
  u32 mr;
  u32 wumr;
  u32 wuir;
  const u32 sr;
} supc_regs_t;

extern supc_regs_t Supc;

typedef volatile struct {
  u32 cr;
  u32 mr;
  const u32 sr;
} wdt_regs_t;

extern wdt_regs_t Wdt;

typedef volatile struct {
  u32 mr;
  u32 ar;
  const u32 vr;
  const u32 sr;
} rtt_regs_t;

extern rtt_regs_t Rtt;

typedef volatile struct {
  u32 cr;
  u32 mr;
  u32 timr;
  u32 calr;
  u32 timalr;
  u32 calalr;
  const u32 sr;
  u32 sccr;
  u32 ier;
  u32 idr;
  const u32 imr;
  const u32 ver;
  const u32 reserved1[45];
  u32 wpmr;
} rtc_regs_t;

extern rtc_regs_t Rtc;

typedef volatile struct {
  u32 gpbr0;
  u32 gpbr1;
  u32 gpbr2;
  u32 gpbr3;
  u32 gpbr4;
  u32 gpbr5;
  u32 gpbr6;
  u32 gpbr7;
} gpbr_regs_t;

extern gpbr_regs_t Gpbr;

typedef volatile struct {
  u32 iser[2];
  const u32 reserved1[30];
  u32 icer[2];
  const u32 reserved2[30];
  u32 ispr[2];
  const u32 reserved3[30];
  u32 icpr[2];
  const u32 reserved4[30];
  const u32 iabr[2];
  const u32 reserved5[62];
  u32 ipr[8];
  const u32 reserved6[696];
  u32 stir;
} nvic_regs_t;

extern nvic_regs_t Nvic;
