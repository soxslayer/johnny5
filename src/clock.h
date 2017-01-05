#pragma once

#include "types.h"

extern u64 clock_speed;

void init_eefc();
void enable_slow_xtal();
void enable_main_xtal();
void enable_plla();
void switch_to_plla();
