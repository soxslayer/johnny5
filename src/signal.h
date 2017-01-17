#pragma once

#include "heap.h"
#include "task.h"

typedef struct _signal_t
{
  heap_t wait_list;
} signal_t;

void signal_init(signal_t *sig);
void signal_wait(signal_t *sig);
void signal_enqueue(signal_t *sig);
void signal_wake_one(signal_t *sig);
void signal_wake_all(signal_t *sig);
