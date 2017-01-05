#include "init.h"
#include "fault.h"
#include "nvic.h"

#define STACK_BOTTOM 0x20088000

void *vector_table[] __attribute((section(".vector_tbl"))) = {
  (void *)STACK_BOTTOM,
  (void *)init,
  (void *)bad_int,
  (void *)fault_handler,
  (void *)fault_handler,
  (void *)fault_handler,
  (void *)fault_handler,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int,
  (void *)bad_int
};
