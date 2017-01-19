
.extern do_schedule

.global ctx_switch
.thumb_func
.thumb
.syntax unified
ctx_switch:
  @ move back to psp
  mov r1, sp
  mrs r0, psp
  mov sp, r0

  @ save off the rest of the task context and prepare argument for do_schedule
  push {r4-r11}
  mov r0, sp

  @ go back to the msp
  mov sp, r1

  @ call scheduler logic
  push {lr}
  bl do_schedule
  pop {lr}

  @ switch new task psp
  mov r1, sp
  mov sp, r0

  @ restore new task context
  pop {r4-r11}

  @ set new context psp
  mov r0, sp
  msr psp, r0

  @ switch back to the msp
  mov sp, r1

  bx lr

.global ctx_exec
.thumb_func
.thumb
.syntax unified
ctx_exec:
  @ switch to the psp and set its value
  mov r1, #2
  msr control, r1
  mov sp, r0

  @ load in the context
  pop {r4-r11}
  pop {r0-r3}
  @ the rest of the context, minus the ret address is discarded

  @ discard r12 and lr
  add sp, #8
  @ get the ret addr
  pop {lr}
  @ discard psr
  add sp, #4

  bx lr
