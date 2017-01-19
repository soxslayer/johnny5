#pragma once

#include "types.h"

#define NAKED __attribute__((naked))
#define ISR __attribute__((isr))

#define IRQ_VECTOR(n) ((n) + 16)

#define NVIC_RESET 1
#define NVIC_HARD_FAULT 3
#define NVIC_MEM_MGT_FAULT 4
#define NVIC_BUS_FAULT 5
#define NVIC_USAGE_FAULT 6
#define NVIC_SVCALL 11
#define NVIC_PENDSV 14
#define NVIC_SYSTICK 15

typedef void (*int_handler_t)();

ISR void bad_int();

void nvic_init();
void nvic_disable_int();
void nvic_enable_int();
void nvic_set_handler(u8 int_num, int_handler_t handler);
