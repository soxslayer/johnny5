#include "main.h"

#include "assert.h"
#include "basic_uart.h"
#include "spinlock.h"
#include "task.h"

spinlock_t sem;

int slow()
{
  while (1) {
    spinlock_acquire(&sem);

    basic_uart_tx_str("slow ");

    for (short i = 1; i; ++i);

    spinlock_release(&sem);

    task_checkpoint();
  }

  return 0;
}

int medium()
{
  while (1) {
    spinlock_acquire(&sem);

    basic_uart_tx_str("medium ");

    spinlock_release(&sem);

    task_checkpoint();
  }

  return 0;
}

int fast()
{
  while (1) {
    spinlock_acquire(&sem);

    basic_uart_tx_str("fast ");

    spinlock_release(&sem);

    task_checkpoint();
  }

  return 0;
}

int main()
{
  spinlock_init(&sem);

  basic_uart_tx_str("Johnny 5 alive!!!\r\n");

  task_add(slow, 1024, 1000, "slow");
  task_add(medium, 1024, 100, "medium");
  task_add(fast, 1024, 10, "fast");

  while (1) {
    task_checkpoint();
  }

  return 0;
}
