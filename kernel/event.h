#pragma once

#include "signal.h"
#include "types.h"

typedef u32 event_t;

typedef struct _event_queue_t
{
  u32 event_id __attribute__((aligned(4)));
  event_t *events;
  size_t n_events;
  u32 pending;
  signal_t wait_queue;
} event_queue_t;

void event_queue_init(event_queue_t *que);
event_t * event_queue_create_event(event_queue_t *que);
void event_queue_signal(event_queue_t *que, event_t *evt);
event_t * event_queue_wait(event_queue_t *que);
