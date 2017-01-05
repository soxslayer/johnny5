#include "idle.h"

#include "main.h"
#include "nvic.h"
#include "task.h"

int NAKED idle()
{
  task_add(main, 1024, 100, "main");

  while (1) task_yield();

  return 0;
}
