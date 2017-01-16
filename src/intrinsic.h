#pragma once

#include "types.h"

#define BKPT

/* returns *ptr */
u32 ldrex(u32 *ptr);
void clrex();
/*returns 0 on success, else 1 */
int strex(u32 v, u32 *ptr);
void dmb();
void dsb();
void isb();
void nop();
void bkpt();
void cpsie_i();
void cpsid_i();
