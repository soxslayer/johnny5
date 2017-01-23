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
#include "syscall.h"
#include "util.h"

#define MS_PER_JIFFY 1
#define MIN_STACK_SIZE 128
#define TASK_PSR 0x01000000
#define TASK_EXCEPT_RETURN_LR 0xfffffff1 // Thread mode PSP stack lr value
#define TASK_EXCEPT_RETURN_THREAD_MODE 0x8
#define TASK_EXCEPT_RETURN_PSP 0x4
#define MAX_MISSED_DEADLINES 5


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
  task->sched.status = TASK_STATUS_RUNNING;
  task->return_value = ep();
  task->sched.status = TASK_STATUS_STOPPED;
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
  tb->priority.period = run_period_ms * MS_PER_JIFFY;
  tb->blocking_task = NULL;
  tb->name[strncpy(tb->name, name, TASK_NAME_SIZE)] = 0;
  tb->parent_task = __cur_task;
  list_init(&tb->child_tasks);
  tb->active_signal = SIG_NONE;

  tb->sched.run_ctx = (task_img_t *)(tb->stack_top + stack_size);
  tb->sched.status = TASK_STATUS_INIT;

  tb->sig_sched.status = TASK_STATUS_INIT;

  for (u32 i = 0; i < NELEMS(tb->sig_mask); ++i) {
    tb->sig_mask[i] = 0;
    tb->sig_pending[i] = 0;
  }

  for (u32 i = 0; i < SIG_MAX; ++i)
    tb->sig_handlers[i] = NULL;

  return tb;
}

static scheduler_context_t * task_get_scheduler_context(task_t *task)
{
  if (task->active_signal == SIG_NONE)
    return &task->sched;

  return &task->sig_sched;
}

static void set_task_image(task_t *task, task_img_t *img)
{
  img->psr = TASK_PSR;
  img->except_lr = TASK_EXCEPT_RETURN_LR
                   | TASK_EXCEPT_RETURN_THREAD_MODE
                   | TASK_EXCEPT_RETURN_PSP;

  scheduler_context_t *ctx = task_get_scheduler_context(task);

  ctx->run_ctx = (task_img_t *)((u8 *)ctx->run_ctx - sizeof(*img));
  memcpy(ctx->run_ctx, img, sizeof(*img));
  ctx->run_ctx->sp = (u32)ctx->run_ctx;
}

/* thread context signal entry point */
static void signal_entry(signal_handler_t handler, signal_id_t sig)
{
  handler(sig);

  __cur_task->sig_sched.status = TASK_STATUS_STOPPED;
  task_yield();
  PANIC();
}

static signal_id_t task_get_pending_signal(task_t *task)
{
  for (u32 i = 0; i < NELEMS(__cur_task->sig_pending); ++i) {
    if (__cur_task->sig_pending[i])
      return i * 32 + find_set(__cur_task->sig_pending[i]);
  }

  return SIG_NONE;
}

static void task_handle_signal(task_t *task)
{
  signal_id_t sig_num = task_get_pending_signal(task);

  if (sig_num != SIG_NONE) {
    clr_bits(__cur_task->sig_pending[sig_num / 32], _b(sig_num % 32));

    if (task->sig_handlers[sig_num] != NULL) {
      task_img_t sig_img;
      memzero(&sig_img, sizeof(sig_img));
      sig_img.r0 = (u32)task->sig_handlers[sig_num];
      sig_img.r1 = sig_num;
      sig_img.ret = (u32)signal_entry;

      task->active_signal = sig_num;

      task->sig_sched.run_ctx = task->sched.run_ctx;
      task->sig_sched.status = TASK_STATUS_RUNNING;

      set_task_image(task, &sig_img);
    }
  }
}

task_img_t * do_schedule(task_img_t *run_ctx)
{
  scheduler_context_t *ctx = task_get_scheduler_context(__cur_task);

  /* if done signal handling discard the task context given in run_ctx */
  if (__cur_task->active_signal != SIG_NONE
      && __cur_task->sig_sched.status == TASK_STATUS_STOPPED) {
    __cur_task->active_signal = SIG_NONE;
    ctx = task_get_scheduler_context(__cur_task);
  }
  else
    ctx->run_ctx = run_ctx;

  if (bits_set(SysTick.ctrl, _b(16)))
    ++__jiffies;

  switch (ctx->status) {
    case TASK_STATUS_RUNNING:
      if (__cur_task->active_signal != SIG_NONE) {
        heap_insert(&__run_queue,
                    &__cur_task->queue.heap,
                    __jiffies + __cur_task->priority.period);
      }
      else if (__cur_task->priority.period == 0)
        heap_insert(&__run_queue, &__cur_task->queue.heap, __jiffies);
      else {
        if (__jiffies > __cur_task->priority.deadline)
          task_signal(__cur_task, SIG_DEADLINE_MISSED);

        jiffy_t next_run = __cur_task->priority.deadline
                           - __cur_task->priority.period;
        if (next_run > __jiffies) {
          ctx->wake_jiffies = next_run;
          ctx->status = TASK_STATUS_SCHED_WAITING;
          heap_insert(&__pending_queue, &__cur_task->queue.heap, next_run);
        }
        else
          heap_insert(&__run_queue,
                      &__cur_task->queue.heap,
                      __cur_task->priority.deadline);
      }

      break;

    case TASK_STATUS_SCHED_WAITING:
      heap_insert(&__pending_queue,
                  &__cur_task->queue.heap,
                  ctx->wake_jiffies);

      break;

    /* if this case task has been added to an external wait queue */
    case TASK_STATUS_WAIT_PENDING:
      ctx->status = TASK_STATUS_WAITING;

      break;

    case TASK_STATUS_STOPPED:
      list_add(&__dead_queue, &__cur_task->queue.list);

      break;

    default:
      /* any other state is invalid */
      PANIC();
      break;
  }

  while (!heap_empty(&__pending_queue)) {
    task_t *t = containerof(heap_root(&__pending_queue), task_t, queue.heap);
    scheduler_context_t *tc = task_get_scheduler_context(t);
    if (tc->wake_jiffies > __jiffies || tc->wake_jiffies == U32_MAX)
      break;
    heap_remove(&__pending_queue, &t->queue.heap);
    tc->status = TASK_STATUS_RUNNING;
    heap_insert(&__run_queue, &t->queue.heap, t->priority.deadline);
  }

  __cur_task = containerof(heap_root(&__run_queue), task_t, queue.heap);

  if (__cur_task->active_signal == SIG_NONE)
    task_handle_signal(__cur_task);

  ctx = task_get_scheduler_context(__cur_task);

  if (ctx->status == TASK_STATUS_RUNNING) {
    while (__cur_task->blocking_task != NULL) {
      __cur_task = __cur_task->blocking_task;
      ctx = task_get_scheduler_context(__cur_task);
    }
  }

  heap_remove(&__run_queue, &__cur_task->queue.heap);

  return ctx->run_ctx;
}

DEFINE_CTX_HANDLER(sched_systick, do_schedule);

static task_img_t * syscall_task_yield(task_img_t *img)
{
  return (task_img_t *)do_schedule(img);
}

static task_img_t * syscall_save_ctx(task_img_t *img)
{
  task_img_t *dst = (task_img_t *)img->r1;
  memcpy(dst, img, sizeof(*dst));

  return img;
}

static task_img_t *syscall_restore_ctx(task_img_t *img)
{
  task_img_t *src = (task_img_t *)img->r1;
  u8 *dst_sp = (u8 *)img->sp;

  memcpy(dst_sp, src, sizeof(*src));

  return (task_img_t *)dst_sp;
}

void sched_init()
{
  heap_create(&__run_queue);
  heap_create(&__pending_queue);
  list_init(&__dead_queue);
  nvic_set_handler(NVIC_SYSTICK, sched_systick);
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
  task_img_t *img = (task_img_t *)task->sched.run_ctx;
  img->r2 = img->r1;
  img->r1 = img->r0;
  img->r0 = img->ret;
  img->ret = (u32)first_task_entry;

  __cur_task = task;

  syscall(SYSCALL_RESTORE_CTX, img);

  PANIC();
}

void task_init()
{
  syscall_register(SYSCALL_TASK_YIELD, syscall_task_yield);
  syscall_register(SYSCALL_SAVE_CTX, syscall_save_ctx);
  syscall_register(SYSCALL_RESTORE_CTX, syscall_restore_ctx);
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

  nvic_disable_int();

  if (__cur_task != NULL)
    list_add(&__cur_task->child_tasks, &task->parent_list);

  task->priority.deadline = __jiffies + task->priority.period;
  heap_insert(&__run_queue, &task->queue.heap, task->priority.deadline);

  nvic_enable_int();

  return task;
}

void task_yield()
{
  syscall(SYSCALL_TASK_YIELD, NULL);
}

void task_blocked(task_t *blocker, list_t *wait_list)
{
  nvic_disable_int();

  __cur_task->blocking_task = blocker;
  if (wait_list != NULL)
    list_add(wait_list, &blocker->spinlock_node);

  nvic_enable_int();
}

void task_unblocked(list_node_t *block_node)
{
  ASSERT(block_node != NULL);

  task_t *task = containerof(block_node, task_t, spinlock_node);

  nvic_disable_int();

  task->blocking_task = NULL;

  nvic_enable_int();
}

void task_checkpoint()
{
  nvic_disable_int();

  __cur_task->priority.deadline += __cur_task->priority.period;

  nvic_enable_int();

  task_yield();
}

static int task_return(task_t *task, int *val)
{
  if (task->sched.status != TASK_STATUS_STOPPED)
    return -1;

  *val = task->return_value;

  return 0;
}

int task_reap(task_t *task)
{
  ASSERT(task != NULL);

  if (task->parent_task != __cur_task)
    return -1;

  if (task->sched.status != TASK_STATUS_STOPPED)
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

  scheduler_context_t *ctx = task_get_scheduler_context(task);

  ASSERT(ctx->status != TASK_STATUS_WAITING
         && ctx->status != TASK_STATUS_WAIT_PENDING);

  if (!heap_contains(&__run_queue, &task->queue.heap))
    heap_insert(&__run_queue, &task->queue.heap, task->priority.deadline);

  nvic_enable_int();
}

void task_unschedule(task_t *task)
{
  ASSERT(task != NULL);

  nvic_disable_int();

  scheduler_context_t *ctx = task_get_scheduler_context(task);

  ctx->status = TASK_STATUS_WAIT_PENDING;
  if (heap_contains(&__run_queue, &task->queue.heap))
    heap_remove(&__run_queue, &task->queue.heap);
  else if (heap_contains(&__pending_queue, &task->queue.heap))
    heap_remove(&__pending_queue, &task->queue.heap);

  nvic_enable_int();
}

void task_reschedule(task_t *task)
{
  ASSERT(task != NULL);

  nvic_disable_int();

  if (task_get_scheduler_context(task)->status == TASK_STATUS_RUNNING) {
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

void task_set_signal_handler(task_t *task,
                             signal_handler_t handler,
                             signal_id_t signal)
{
  ASSERT(task != NULL && signal < SIG_MAX);

  nvic_disable_int();

  task->sig_handlers[signal] = handler;

  nvic_enable_int();
}

void task_signal(task_t *task, signal_id_t signal)
{
  ASSERT(task != NULL && signal < SIG_MAX);

  u32 idx = signal / 32;
  u32 bit = signal % 32;

  nvic_disable_int();

  set_bits(task->sig_pending[idx], _b(bit));

  if (task->sched.status == TASK_STATUS_SCHED_WAITING) {
    heap_remove(&__pending_queue, &task->queue.heap);
    /* use period as deadline so some sort of priority is still maintained */
    heap_insert(&__run_queue,
                &task->queue.heap,
                __jiffies + task->priority.period + 1);
  }

  nvic_enable_int();
}

void task_wait_signal()
{
  nvic_disable_int();

  /* if there's a pending signal no need to wait */
  if (task_get_pending_signal(__cur_task) == SIG_NONE) {
    __cur_task->sched.status = TASK_STATUS_SCHED_WAITING;
    __cur_task->sched.wake_jiffies = U32_MAX;
  }

  nvic_enable_int();

  task_yield();
}

void task_sleep(u32 ms)
{
  nvic_disable_int();

  scheduler_context_t *ctx = task_get_scheduler_context(__cur_task);

  ctx->status = TASK_STATUS_SCHED_WAITING;
  ctx->wake_jiffies = __jiffies + ms * MS_PER_JIFFY;

  nvic_enable_int();

  task_yield();
}
