#include "nvic.h"

#include "basic_uart.h"
#include "atomic.h"
#include "sam3x8e.h"
#include "task.h"
#include "types.h"

static void print_except_state(task_img_t *img)
{
  basic_uart_tx_str(" sp: ");
  basic_uart_tx_u32((u32)img);
  basic_uart_tx_str("\r\n r0: ");
  basic_uart_tx_u32(img->r0);
  basic_uart_tx_str(" r1: ");
  basic_uart_tx_u32(img->r1);
  basic_uart_tx_str(" r2: ");
  basic_uart_tx_u32(img->r2);
  basic_uart_tx_str(" r3: ");
  basic_uart_tx_u32(img->r3);
  basic_uart_tx_str("\r\n r4: ");
  basic_uart_tx_u32(img->r4);
  basic_uart_tx_str(" r5: ");
  basic_uart_tx_u32(img->r5);
  basic_uart_tx_str(" r6: ");
  basic_uart_tx_u32(img->r6);
  basic_uart_tx_str(" r7: ");
  basic_uart_tx_u32(img->r7);
  basic_uart_tx_str("\r\n r8: ");
  basic_uart_tx_u32(img->r8);
  basic_uart_tx_str(" r9: ");
  basic_uart_tx_u32(img->r9);
  basic_uart_tx_str(" r10: ");
  basic_uart_tx_u32(img->r10);
  basic_uart_tx_str(" r11: ");
  basic_uart_tx_u32(img->r11);
  basic_uart_tx_str("\r\n r12: ");
  basic_uart_tx_u32(img->r12);
  basic_uart_tx_str(" lr: ");
  basic_uart_tx_u32(img->lr);
  basic_uart_tx_str(" ret: ");
  basic_uart_tx_u32(img->ret);
  basic_uart_tx_str(" psp: ");
  basic_uart_tx_u32(img->psr);
  basic_uart_tx_str("\r\n MMSR: ");
  basic_uart_tx_u32(Scb.mmsr);
  basic_uart_tx_str(" BFSR: ");
  basic_uart_tx_u32(Scb.bfsr);
  basic_uart_tx_str(" UFSR: ");
  basic_uart_tx_u32(Scb.ufsr);
  basic_uart_tx_str(" HFSR: ");
  basic_uart_tx_u32(Scb.hfsr);
  basic_uart_tx_str("\r\nBFAR: ");
  basic_uart_tx_u32(Scb.bfar);
}

ISR void bad_int()
{
  while (1)
    basic_uart_tx_str("bad int\r\n");
}

void hard_fault_handler(task_img_t *img)
{
  basic_uart_tx_str("\r\n\r\nhard fault\r\n");
  print_except_state(img);
  while (1);
}

void memory_management_fault_handler(task_img_t *img)
{
  basic_uart_tx_str("\r\n\r\nmemory management fault\r\n");
  print_except_state(img);
  while (1);
}

void bus_fault_handler(task_img_t *img)
{
  basic_uart_tx_str("\r\n\r\nbus fault\r\n");
  print_except_state(img);
  while (1);
}

void usage_fault_handler(task_img_t *img)
{
  basic_uart_tx_str("\r\n\r\nusage fault\r\n");
  print_except_state(img);
  while (1);
}

extern char __vectortbl_start__;
extern char __vectortbl_end__;
extern char __vectortbl_load__;

/* move the interrupt vector table to SRAM */
void nvic_init()
{
  long size = &__vectortbl_end__ - &__vectortbl_start__;

  for (int i = 0; i < size; ++i)
    (&__vectortbl_start__)[i] = (&__vectortbl_load__)[i];

  Scb.vtor = _b(29) | (u32)(&__vectortbl_start__);
}

static u32 __int_disable_count = 0;

void nvic_disable_int()
{
  asm("cpsid i");
  asm("dsb");

  ++__int_disable_count;
}

void nvic_enable_int()
{
  if (--__int_disable_count == 0) {
    asm("dsb");
    asm("cpsie i");
  }
}

void nvic_set_handler(u8 int_num, int_handler_t handler)
{
  ((int_handler_t *)(&__vectortbl_start__))[int_num] = handler;
}
