#pragma once

#include "heap.h"
#include "list.h"
#include "types.h"

#define TASK_SELF NULL

typedef enum {
  TASK_STATUS_INIT,
  TASK_STATUS_RUNNING,
  TASK_STATUS_STOPPED,
  TASK_STATUS_WAIT_PENDING,
  TASK_STATUS_WAITING
} task_status_t;

typedef int (*task_entry_t)();

typedef struct _task_img_t
{
  /* manually saved by context systick interrupt */
  u32 r4;
  u32 r5;
  u32 r6;
  u32 r7;
  u32 r8;
  u32 r9;
  u32 r10;
  u32 r11;
  /* automatically saved upon exception entry */
  u32 r0;
  u32 r1;
  u32 r2;
  u32 r3;
  u32 r12;
  u32 lr;
  u32 ret;
  u32 psr;
} task_img_t;

#define TASK_NAME_SIZE 16

typedef struct _task_priority_t
{
  jiffy_t period;
  jiffy_t deadline;
} task_priority_t;

typedef struct _task_priority_node_t
{
  task_priority_t priority;
  heap_node_t node;
} task_priority_node_t;

typedef struct _task_t
{
  union
  {
    heap_node_t heap;
    list_node_t list;
  } queue;
  u8 *stack_top;
  u8 *psp;
  task_priority_t priority;
  heap_t promotions;
  int return_value;
  list_t child_tasks;
  struct _task_t *parent_task;
  struct _task_t *blocking_task;
  list_node_t parent_list;
  list_node_t spinlock_node;
  task_status_t status;
  char name[TASK_NAME_SIZE + 1];
  int missed_deadlines;
} task_t;

u8 * do_schedule(u8 *psp);
void sched_init();
void sched_start(task_entry_t ep, u32 stack_size);

task_t * task_add(task_entry_t ep, u32 stack_size, u32 run_period_ms,
                  const char *name);
void task_remove(task_t *task);
void task_yield();
void task_blocked(task_t *blocker);
void task_unblocked(task_t *task);
void task_checkpoint();
void task_schedule(task_t *task);
void task_unschedule(task_t *task);
void task_reschedule(task_t *task);
task_t * task_self();
task_priority_t * task_get_priority(task_t *task);
void task_set_period(task_t *task, u32 period_ms);
