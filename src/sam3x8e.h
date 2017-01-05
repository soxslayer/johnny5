#pragma once

#include "bits.h"
#include "types.h"

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
} uart_def_t;

extern uart_def_t Uart0;

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
} pmc_t;

extern pmc_t Pmc;

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
} pio_t;

extern pio_t PioA;
extern pio_t PioB;
extern pio_t PioC;
extern pio_t PioD;
extern pio_t PioE;
extern pio_t PioF;

typedef volatile struct {
  u32 fmr;
  u32 fcr;
  const u32 fsr;
  const u32 frr;
} eefc_t;

extern eefc_t Effc0;
extern eefc_t Effc1;

typedef volatile struct {
  u32 cr;
  u32 mr;
  const u32 sr;
} wdt_t;

extern wdt_t Wdt;

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
} scb_t;

extern scb_t Scb;

typedef volatile struct {
  u32 ctrl;
  u32 load;
  u32 val;
  const u32 calib;
} systick_t;

extern systick_t SysTick;

typedef volatile struct {
  u32 cr;
  u32 smmr;
  u32 mr;
  u32 wumr;
  u32 wuir;
  const u32 sr;
} supc_t;

extern supc_t SupC;
