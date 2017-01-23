
.global ctx_exec
.thumb_func
.thumb
.syntax unified
ctx_exec:
  // switch to the psp and set its value
  mov r1, #2
  msr control, r1
  mov sp, r0

  // discard the sp from the context
  add sp, #4
  // load in the context
  pop {r4-r11}
  pop {r0-r3}
  // the rest of the context, minus the ret address is discarded

  // discard r12 and lr
  add sp, #8
  // get the ret addr
  pop {lr}
  // discard psr
  add sp, #4

  bx lr

// save and restore context
// pass function to call between saving and restoring context in r0
.global ctx_save
.thumb_func
.thumb
.syntax unified
ctx_save:
  // figure out which stack pointer to get
  ands r1, lr, #4
  itte eq
  mrseq r1, msp
  subeq sp, sp, #40 // sp == msp, need to allocate space for rest of context
  mrsne r1, psp

  // ready sp to be pushed into saved context
  mov r3, r1
  sub r3, r3, #40

  // ready lr to be pushed into saved context
  mov r2, lr

  // save rest of context
  stmdb r1!, {r2-r11}

  // call handler function passed in
  mov r3, r0
  mov r0, r1
  blx r3

  // restore context
  ldm r0, {r2-r11}

  // ready lr
  mov lr, r2

  // ready new sp
  add r3, r3, #40

  // save new sp
  ands r1, lr, #4
  ite eq
  msreq msp, r3
  msrne psp, r3

  // gtfo
  bx lr
