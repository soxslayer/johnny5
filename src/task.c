#include "task.h"

#include "assert.h"
#include "atomic.h"
#include "clock.h"
#include "ctx_switch.h"
#include "limits.h"
#include "mem.h"
#include "nvic.h"
#include "sam3x8e.h"
#include "sem.h"
#include "string.h"
#include "util.h"

#define MS_PER_JIFFY 1
#define MIN_STACK_SIZE 128
#define TASK_PSR 0x01000000
#define MAX_MISSED_DEADLINES 5

#define TASK_REMOVE _b(0)

/* modified from interrupt context or with them disabled */
heap_t __run_queue;
heap_t __pending_queue;
list_t __dead_queue;
jiffy_t __jiffies = 0;
task_t *__cur_task = NULL;

/* entry point for the very first task called */
static void first_task_entry(task_entry_t ep, task_entry_t old_ep, task_t *task)
{
  sem_post();

  SysTick.load = clock_speed / (1000 * MS_PER_JIFFY) - 1;
  SysTick.ctrl = _b(2) | _b(1) | _b(0);

  ep(old_ep, task);
}

static void task_entry(task_entry_t ep, task_t *task)
{
  task->status = TASK_STATUS_RUNNING;
  task->return_value = ep();
  task->status = TASK_STATUS_STOPPED;
  task_yield();
  PANIC();
}

static task_t * create_task_entry(u32 stack_size, u32 run_period_ms,
                                  const char *name)
{
  task_t *tb = malloc(sizeof(*tb));

  if (stack_size < MIN_STACK_SIZE)
    stack_size = MIN_STACK_SIZE;

  tb->stack_top = a_malloc(stack_size, 4);
  tb->psp = tb->stack_top + stack_size;
  tb->priority.period = run_period_ms * MS_PER_JIFFY;
  heap_init(&tb->promotions);
  tb->name[strncpy(tb->name, name, TASK_NAME_SIZE)] = 0;
  tb->status = TASK_STATUS_INIT;
  tb->parent_task = __cur_task;
  tb->blocking_task = NULL;
  list_init(&tb->child_tasks);
  tb->missed_deadlines = 0;

  return tb;
}

static void set_task_image(task_t *task, task_img_t *img)
{
  task->psp -= sizeof(*img);

  img->psr = TASK_PSR;

  memcpy(task->psp, img, sizeof(*img));
}

u8 * do_schedule(u8 *psp)
{
  __cur_task->psp = psp;

  if (bits_set(SysTick.ctrl, _b(16)))
    ++__jiffies;

  switch (__cur_task->status) {
    case TASK_STATUS_RUNNING:
      if (__cur_task->priority.period == 0)
        heap_insert(&__run_queue, &__cur_task->queue.heap, __jiffies);
      else {
        if (__jiffies > __cur_task->priority.deadline)
          ++__cur_task->missed_deadlines;

        ASSERT(__cur_task->missed_deadlines < MAX_MISSED_DEADLINES);

        jiffy_t next_run = __cur_task->priority.deadline
                           - __cur_task->priority.period;
        if (next_run > __jiffies)
          heap_insert(&__pending_queue, &__cur_task->queue.heap, next_run);
        else
          heap_insert(&__run_queue, &__cur_task->queue.heap,
                      __cur_task->priority.deadline);
      }

      break;

    case TASK_STATUS_WAIT_PENDING:
      __cur_task->status = TASK_STATUS_WAITING;

      break;

    case TASK_STATUS_STOPPED:
      list_add(&__dead_queue, &__cur_task->queue.list);

      break;

    default:
      break;
  }

  while (!heap_empty(&__pending_queue)) {
    task_t *t = containerof(heap_root(&__pending_queue), task_t, queue.heap);
    if (t->priority.deadline > __jiffies
        && t->priority.deadline - t->priority.period > __jiffies)
      break;
    heap_remove(&__pending_queue, &t->queue.heap);
    heap_insert(&__run_queue, &t->queue.heap, t->priority.deadline);
  }

  __cur_task = containerof(heap_root(&__run_queue), task_t, queue.heap);
  while (__cur_task->blocking_task != NULL)
    __cur_task = __cur_task->blocking_task;

  heap_remove(&__run_queue, &__cur_task->queue.heap);

  return __cur_task->psp;
}

void sched_init()
{
  heap_init(&__run_queue);
  heap_init(&__pending_queue);
  list_init(&__dead_queue);
  nvic_set_handler(NVIC_SYSTICK, ctx_switch);
  nvic_set_handler(NVIC_SVCALL, ctx_switch);
  Scb.shpr2 = 0;
  Scb.shpr3 = 0;
}

void sched_start(task_entry_t ep, u32 stack_size)
{
  ASSERT(ep != NULL);

  task_add(ep, stack_size, U32_MAX, "idle");

  task_t *task = containerof(heap_root(&__run_queue), task_t, queue.heap);
  heap_remove(&__run_queue, &task->queue.heap);

  /* the first task requires a special image to call first_task_entry */
  task_img_t *img = (task_img_t *)task->psp;
  img->r2 = img->r1;
  img->r1 = img->r0;
  img->r0 = img->ret;
  img->ret = (u32)first_task_entry;

  __cur_task = task;

  ctx_exec(task->psp);

  PANIC();
}

task_t * task_add(task_entry_t ep, u32 stack_size, u32 run_period_ms,
                  const char *name)
{
  ASSERT(ep != NULL);

  task_t *task = create_task_entry(stack_size, run_period_ms, name);

  task_img_t img;
  memzero(&img, sizeof(img));
  img.r0 = (u32)ep;
  img.r1 = (u32)task;
  img.ret = (u32)task_entry;
  set_task_image(task, &img);

  if (__cur_task != NULL)
    list_add(&__cur_task->child_tasks, &task->parent_list);

  nvic_disable_int();

  task->priority.deadline = __jiffies + task->priority.period;
  task_schedule(task);

  nvic_enable_int();

  return task;
}

void task_yield()
{
  asm("svc 0");
}

void task_blocked(task_t *blocker)
{
  nvic_disable_int();

  __cur_task->blocking_task = blocker;

  nvic_enable_int();
}

void task_unblocked(task_t *task)
{
  ASSERT(task != NULL);

  nvic_disable_int();

  task->blocking_task = NULL;

  nvic_enable_int();
}

void task_checkpoint()
{
  nvic_disable_int();

  ASSERT(heap_empty(&__cur_task->promotions));

  __cur_task->priority.deadline += __cur_task->priority.period;
  __cur_task->missed_deadlines = 0;

  nvic_enable_int();

  task_yield();
}

static int task_return(task_t *task, int *val)
{
  if (task->status != TASK_STATUS_STOPPED)
    return -1;

  *val = task->return_value;

  return 0;
}

int task_reap(task_t *task)
{
  ASSERT(task != NULL);

  if (task->parent_task != __cur_task)
    return -1;

  if (task->status != TASK_STATUS_STOPPED)
    return -1;

  list_foreach(&task->child_tasks, child) {
    list_add(&task->parent_task->child_tasks, child);
  }

  list_remove(&task->parent_task->child_tasks, &task->parent_list);
  a_free(task->stack_top);
  free(task);

  return 0;
}

void task_schedule(task_t *task)
{
  ASSERT(task != NULL);

  nvic_disable_int();

  if (task->status == TASK_STATUS_WAITING || task->status == TASK_STATUS_INIT)
    heap_insert(&__run_queue, &task->queue.heap, task->priority.deadline);

  task->status = TASK_STATUS_RUNNING;

  nvic_enable_int();
}

void task_unschedule(task_t *task)
{
  ASSERT(task != NULL);

  nvic_disable_int();

  task->status = TASK_STATUS_WAIT_PENDING;
  if (task != __cur_task && task->status == TASK_STATUS_RUNNING)
    heap_remove(&__run_queue, &task->queue.heap);

  nvic_enable_int();
}

void task_reschedule(task_t *task)
{
  ASSERT(task != NULL);

  nvic_disable_int();

  if (task->status == TASK_STATUS_RUNNING) {
    task_unschedule(task);
    task_schedule(task);
  }

  nvic_enable_int();
}

task_t * task_self()
{
  return __cur_task;
}

task_priority_t * task_get_priority(task_t *task)
{
  ASSERT(task != NULL);

  return &task->priority;
}

void task_set_period(task_t *task, u32 period_ms)
{
  ASSERT(task != NULL);

  nvic_disable_int();

  task->priority.period = period_ms * MS_PER_JIFFY;
  task_reschedule(task);

  nvic_enable_int();
}
