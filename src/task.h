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

typedef struct _task_t
{
  union
  {
    heap_node_t run;
    list_node_t wait;
  } queue;
  u8 *stack_top;
  u8 *psp;
  heap_t blocking;
  jiffy_t run_period_jiffies;
  jiffy_t deadline_jiffies;
  int return_value;
  list_t child_tasks;
  struct _task_t *parent_task;
  list_node_t parent_list;
  task_status_t status;
  char name[9];
} task_t;

u8 * do_schedule(u8 *psp);
void sched_init();
void sched_start(task_entry_t ep, u32 stack_size);

task_t * task_add(task_entry_t ep, u32 stack_size, u32 run_period_ms,
                  const char *name);
void task_remove(task_t *task);
void task_yield();
void task_checkpoint();
void task_schedule(task_t *task);
void task_unschedule(task_t *task);
task_t * task_self();
void task_set_period(task_t *task, u32 period_ms);
void task_promote(task_t *task, heap_node_t *obj, task_t *promote_to);
void task_demote(task_t *task, heap_node_t *obj);
