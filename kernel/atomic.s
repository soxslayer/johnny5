.thumb

.global atomic_dec
.thumb_func
atomic_dec:
  ldrex r2, [r0]
  sub r2, r1
  strex r3, r2, [r0]
  cmp r3, #0
  bne atomic_dec
  mov r2, r0
  bx lr

.global atomic_inc
.thumb_func
atomic_inc:
  ldrex r2, [r0]
  add r2, r1
  strex r3, r2, [r0]
  cmp r3, #0
  bne atomic_inc
  mov r2, r0
  bx lr

.global atomic_cas
.thumb_func
atomic_cas:
  ldrex r3, [r0]
  cmp r3, r2
  beq __atomic_cas1
  mov r0, #0
  clrex
  bx lr
__atomic_cas1:
  strex r3, r1, [r0]
  cmp r3, #1
  beq atomic_cas
  mov r0, #1
  bx lr
