#pragma once

#include "sam3x8e.h"

typedef enum {
  SUPC_ID,
  RSTC_ID,
  RTC_ID,
  RTT_ID,
  WDT_ID,
  PMC_ID,
  EEFC0_ID,
  EEFC1_ID,
  UART_ID,
  SMC_SDRAMC_ID,
  SDRAMC_ID,
  PIOA_ID,
  PIOB_ID,
  PIOC_ID,
  PIOD_ID,
  PIOE_ID,
  PIOF_ID,
  USART0_ID,
  USART1_ID,
  USART2_ID,
  USART3_ID,
  HSMCI_ID,
  TWI0_ID,
  TWI1_ID,
  SPI0_ID,
  SPI1_ID,
  SSC_ID,
  TC0_ID,
  TC1_ID,
  TC2_ID,
  TC3_ID,
  TC4_ID,
  TC5_ID,
  TC6_ID,
  TC7_ID,
  TC8_ID,
  PWM_ID,
  ADC_ID,
  DAAC_ID,
  DMAC_ID,
  UOTGHS_ID,
  TRNG_ID,
  EMAC_ID,
  CAN0_ID,
  CAN1_ID
} peripheral_id_t;

void peripheral_enable_clock(peripheral_id_t per);
void peripheral_disable_clock(peripheral_id_t per);
void peripheral_enable_int(peripheral_id_t per);
void peripheral_disable_int(peripheral_id_t per);
