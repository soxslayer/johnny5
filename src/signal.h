#pragma once

#include "list.h"
#include "task.h"

#define SIGNAL_INIT { .wait_list = LIST_INIT }

typedef struct _signal_t
{
  list_t wait_list;
} signal_t;

void signal_init(signal_t *sig);
void signal_wait(signal_t *sig);
void signal_enqueue(signal_t *sig);
void signal_wake_one(signal_t *sig);
void signal_wake_all(signal_t *sig);
