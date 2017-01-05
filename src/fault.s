
.extern hard_fault_handler
.extern memory_management_fault_handler
.extern bus_fault_handler
.extern usage_fault_handler

.global fault_handler
.thumb_func
.thumb
.syntax unified
fault_handler:
  ands r0, lr, #4
  ite eq
  mrseq r0, msp
  mrsne r0, psp

  stmdb r0!, {r4-r11}

  mrs r1, psr
  lsl r1, r1, #23
  lsr r1, r1, #23

  push {lr}

  cmp r1, #3
  it eq
  bleq hard_fault_handler

  cmp r1, #4
  it eq
  bleq memory_management_fault_handler

  cmp r1, #5
  it eq
  bleq bus_fault_handler

  cmp r1, #6
  it eq
  bleq usage_fault_handler

  pop {lr}

  bx lr
