#include "idle.h"

#include "chardev.h"
#include "main.h"
#include "task.h"
#include "uart.h"

int NAKED idle()
{
  chardev_init();
  uart_init();

  task_add(main, 1024, 100, "main");

  while (1) task_yield();

  return 0;
}
