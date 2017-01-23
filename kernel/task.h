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
  TASK_STATUS_WAITING,
  TASK_STATUS_SCHED_WAITING
} task_status_t;

typedef int (*task_entry_t)();

/* changes to this structure require changes to all context saving assembly */
typedef struct _task_img_t
{
  /* manually saved by context systick interrupt */
  u32 except_lr;
  u32 sp;
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

typedef enum
{
  SIG_DEADLINE_MISSED,
  SIG_USR1,
  SIG_MAX, /* must be second to last */
  SIG_NONE /* must be last */
} signal_id_t;

typedef void (*signal_handler_t)(signal_id_t);
#define TASK_DEFAULT_SIGNAL_HANDLER NULL
#define SIG_BIT_ARRAY_SIZE ((SIG_MAX + 31) / 32)

struct _task_t;

typedef struct _scheduler_context_t
{
  task_img_t *run_ctx;

  jiffy_t wake_jiffies;

  task_status_t status;
} scheduler_context_t;

typedef struct _task_t
{
  union
  {
    heap_node_t heap;
    list_node_t list;
  } queue;

  scheduler_context_t sched;
  scheduler_context_t sig_sched;

  u8 *stack_top;

  task_priority_t priority;

  int return_value;

  struct _task_t *blocking_task;
  list_node_t spinlock_node;

  list_t child_tasks;
  struct _task_t *parent_task;
  list_node_t parent_list;

  char name[TASK_NAME_SIZE + 1];

  u32 sig_mask[SIG_BIT_ARRAY_SIZE];
  u32 sig_pending[SIG_BIT_ARRAY_SIZE];
  signal_handler_t sig_handlers[SIG_MAX];
  signal_id_t active_signal;
} task_t;

void sched_init();
void sched_start(task_entry_t ep, u32 stack_size);

void task_init();
task_t * task_add(task_entry_t ep, u32 stack_size, u32 run_period_ms,
                  const char *name);
void task_remove(task_t *task);
void task_yield();
void task_blocked(task_t *blocker, list_t *wait_list);
void task_unblocked(list_node_t *block_node);
void task_checkpoint();
void task_schedule(task_t *task);
void task_unschedule(task_t *task);
void task_reschedule(task_t *task);
task_t * task_self();
task_priority_t * task_get_priority(task_t *task);
void task_set_period(task_t *task, u32 period_ms);
void task_set_signal_handler(task_t *task,
                             signal_handler_t handler,
                             signal_id_t signal);
void task_signal(task_t *task, signal_id_t signal);
void task_wait_signal();
void task_sleep(u32 ms);
