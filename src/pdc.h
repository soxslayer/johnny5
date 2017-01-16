#pragma once

#include "buff.h"
#include "list.h"
#include "sam3x8e.h"
#include "types.h"

typedef enum { PDC_TX, PDC_RX, PDC_TXRX } pdc_type_t;

typedef struct _pdc_channel_t
{
  list_t pending;
  list_t complete;
  buff_t *cur_buff;
  buff_t *next_buff;
  pdc_regs_t *hw;
  pdc_type_t type;
} pdc_channel_t;

void pdc_init(pdc_channel_t *pdc, pdc_regs_t *hw, pdc_type_t type);
void pdc_reset(pdc_channel_t *pdc);
void pdc_enable(pdc_channel_t *pdc);
void pdc_disable(pdc_channel_t *pdc);
void pdc_swap_buff(pdc_channel_t *pdc);
void pdc_queue_buff(pdc_channel_t *pdc, buff_t *buff);
buff_t * pdc_dequeue_buff(pdc_channel_t *pdc);
bool pdc_cur_buff_queued(pdc_channel_t *pdc);
bool pdc_next_buff_queued(pdc_channel_t *pdc);
