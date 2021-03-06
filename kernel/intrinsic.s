.thumb

.global ldrex
.thumb_func
ldrex:
  ldrex r0, [r0]
  bx lr

.global clrex
.thumb_func
clrex:
  clrex
  bx lr

.global strex
.thumb_func
strex:
  strex r0, r0, [r1]
  bx lr

.global dmb
.thumb_func
dmb:
  dmb
  bx lr

.global dsb
.thumb_func
dsb:
  dsb
  bx lr

.global isb
.thumb_func
isb:
  isb
  bx lr

.global nop
.thumb_func
nop:
  nop
  bx lr

.global bkpt
.thumb_func
bkpt:
  bkpt
  bx lr

.global cpsie_i
.thumb_func
cpsie_i:
  cpsie i
  bx lr
 
.global cpsid_i
.thumb_func
cpsid_i:
  cpsid i
  bx lr

.global __clear_cache
.thumb_func
__clear_cache:
  isb
  bx lr
