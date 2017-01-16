#include "event.h"

#include "assert.h"
#include "atomic.h"
#include "bits.h"
#include "nvic.h"

void event_queue_init(event_queue_t *que)
{
  ASSERT(que != NULL);

  que->event_id = 0;
  que->events = malloc(8 * sizeof(event_t));
  que->n_events = 8;
  que->pending = 0;

  memzero(que->events, 8 * sizeof(event_t));

  signal_init(&que->wait_queue);
}

event_t * event_queue_create_event(event_queue_t *que)
{
  ASSERT(que != NULL);

  event_t ret = que->event_id++;
  if (ret >= que->n_events) {
    que->events = realloc(que->events, que->n_events * 2);
    memzero(que->events + que->n_events, que->n_events * sizeof(event_t));
    que->n_events *= 2;
  }

  return &que->events[ret];
}

void event_queue_signal(event_queue_t *que, event_t *evt)
{
  ASSERT(que != NULL && evt != NULL);

  int idx = evt - que->events;

  nvic_disable_int();

  if (que->events[idx] == 0) {
    que->events[idx] = 1;
    ++que->pending;
  }

  nvic_enable_int();

  signal_wake_all(&que->wait_queue);
}

event_t * event_queue_wait(event_queue_t *que)
{
  ASSERT(que != NULL);

  while (1) {
    nvic_disable_int();
    if (que->pending == 0) {
      signal_enqueue(&que->wait_queue);
      nvic_enable_int();
      task_yield();
    }
    else {
      nvic_enable_int();
      break;
    }
  }

  event_t *ret;

  for (int i = 0; i < que->event_id; ++i) {
    if (que->events[i] > 0) {
      nvic_disable_int();

      --que->pending;
      que->events[i] = 0;

      nvic_enable_int();

      ret = &que->events[i];

      break;
    }
  }

  return ret;
}
