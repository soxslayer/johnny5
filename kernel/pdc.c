#include "pdc.h"

#include "assert.h"
#include "atomic.h"
#include "bits.h"
#include "mem.h"
#include "nvic.h"
#include "util.h"

static void pdc_enable_key(pdc_channel_t *pdc, int key)
{
  switch (pdc->type) {
    case PDC_TX:
      set_bits(pdc->hw->ptcr, key);
      break;

    case PDC_RX:
      set_bits(pdc->hw->ptcr, key);
      break;

    case PDC_TXRX:
      PANIC();
      break;
  }

  mbarrier();
}

static int pdc_disable_key(pdc_channel_t *pdc)
{
  ASSERT(pdc != NULL);

  int key;

  switch (pdc->type) {
    case PDC_TX:
      key = pdc->hw->ptsr & _b(8);
      set_bits(pdc->hw->ptcr, _b(9));
      break;

    case PDC_RX:
      key = pdc->hw->ptsr & _b(0);
      set_bits(pdc->hw->ptcr, _b(1));
      break;

    case PDC_TXRX:
      PANIC();
      break;
  }

  mbarrier();

  return key;
}

static bool pdc_buff_in_hw(pdc_channel_t *pdc, buff_t *buff)
{
  switch (pdc->type) {
    case PDC_TX:
      if ((u8 *)pdc->hw->tpr >= buff->buff
          && (u8 *)pdc->hw->tpr < buff->buff + buff->buff_size
          && pdc->hw->tcr > 0)
        return true;

      break;

    case PDC_RX:
      if ((u8 *)pdc->hw->rpr >= buff->buff 
          && (u8 *)pdc->hw->rpr < buff->buff + buff->buff_size
          && pdc->hw->rcr > 0)
        return true;

      break;

    case PDC_TXRX:
      PANIC();

      break;
  }

  return false;
}

void pdc_init(pdc_channel_t *pdc, pdc_regs_t *hw, pdc_type_t type)
{
  ASSERT(pdc != NULL && hw != NULL);

  list_init(&pdc->pending);
  pdc->hw = hw;
  pdc->type = type;
  pdc->cur_buff = NULL;
  pdc->next_buff = NULL;
}

void pdc_reset(pdc_channel_t *pdc)
{
  ASSERT(pdc != NULL);

  int key = pdc_disable_key(pdc);

  switch (pdc->type) {
    case PDC_TX:
      pdc->hw->tncr = 0;
      pdc->hw->tnpr = 0;
      pdc->hw->tcr = 0;
      pdc->hw->tpr = 0;

      break;

    case PDC_RX:
      pdc->hw->rncr = 0;
      pdc->hw->rnpr = 0;
      pdc->hw->rcr = 0;
      pdc->hw->rpr = 0;

      break;

    case PDC_TXRX:
      PANIC();
      break;
  }

  pdc_enable_key(pdc, key);
}

void pdc_enable(pdc_channel_t *pdc)
{
  ASSERT(pdc != NULL);

  switch (pdc->type) {
    case PDC_TXRX:
    case PDC_TX:
      pdc_enable_key(pdc, _b(8));

      break;

    case PDC_RX:
      pdc_enable_key(pdc, _b(0));

      break;
  }
}

void pdc_disable(pdc_channel_t *pdc)
{
  pdc_disable_key(pdc);
}

void pdc_swap_buff(pdc_channel_t *pdc)
{
  ASSERT(pdc != NULL);

  nvic_disable_int();
  int key = pdc_disable_key(pdc);

  if (pdc->cur_buff != NULL) {
    buff_t *buff = pdc->cur_buff;

    switch (pdc->type) {
      case PDC_TX:
        if (pdc_buff_in_hw(pdc, buff)) {
          pdc_enable_key(pdc, key);
          nvic_enable_int();

          return;
        }

        buff_reset(buff);

        break;

      case PDC_RX:
        if (pdc_buff_in_hw(pdc, buff)) {
          if (pdc->hw->rcr == buff->buff_size) {
            pdc_enable_key(pdc, key);
            nvic_enable_int();

            return;
          }

          buff->n_wrote = buff->buff_size - pdc->hw->rcr;
          pdc->hw->rcr = 0;
        }
        else
          buff->n_wrote = buff->buff_size;

        break;

      case PDC_TXRX:
        PANIC();
        break;
    }

    list_add(&pdc->complete, &buff->list);
  }

  pdc->cur_buff = pdc->next_buff;

  if (!list_empty(&pdc->pending)) {
    pdc->next_buff = containerof(list_front(&pdc->pending), buff_t, list);

    switch (pdc->type) {
      case PDC_TX:
        pdc->hw->tnpr = pdc->next_buff->buff;
        pdc->hw->tncr = pdc->next_buff->n_wrote;

        break;

      case PDC_RX:
        pdc->hw->rnpr = pdc->next_buff->buff;
        pdc->hw->tncr = pdc->next_buff->buff_size;

        break;

      case PDC_TXRX:
        PANIC();
        break;
    }

    list_pop_front(&pdc->pending);
  }
  else
    pdc->next_buff = NULL;

  pdc_enable_key(pdc, key);
  nvic_enable_int();
}

void pdc_queue_buff(pdc_channel_t *pdc, buff_t *buff)
{
  ASSERT(pdc != NULL && buff != NULL);

  int key = pdc_disable_key(pdc);
  nvic_disable_int();

  switch (pdc->type) {
    case PDC_TX:
      if (pdc->hw->tncr == 0) {
        if (pdc->hw->tcr == 0) {
          pdc->hw->tpr = buff->buff;
          pdc->hw->tcr = buff->n_wrote;
          pdc->cur_buff = buff;
        }
        else {
          pdc->hw->tnpr = buff->buff;
          pdc->hw->tncr = buff->n_wrote;
          pdc->next_buff = buff;
        }

        mbarrier();
      }
      else
        list_push_back(&pdc->pending, &buff->list);

      break;

    case PDC_RX:
      if (pdc->hw->rncr == 0) {
        if (pdc->hw->rcr == 0) {
          pdc->hw->rpr = buff->buff;
          pdc->hw->rcr = buff->buff_size;
          pdc->cur_buff = buff;
        }
        else {
          pdc->hw->rnpr = buff->buff;
          pdc->hw->rncr = buff->buff_size;
          pdc->next_buff = buff;
        }

        mbarrier();
      }
      else
        list_push_back(&pdc->pending, &buff->list);

      break;

    case PDC_TXRX:
      PANIC();
      break;
  }

  nvic_enable_int();
  pdc_enable_key(pdc, key);
}

buff_t * pdc_dequeue_buff(pdc_channel_t *pdc)
{
  buff_t *r = NULL;

  nvic_disable_int();

  if (!list_empty(&pdc->complete)) {
    r = containerof(list_front(&pdc->complete), buff_t, list);
    list_pop_front(&pdc->complete);
  }

  nvic_enable_int();

  return r;
}

bool pdc_cur_buff_queued(pdc_channel_t *pdc)
{
  ASSERT(pdc != NULL);

  return pdc->cur_buff != NULL;
}

bool pdc_next_buff_queued(pdc_channel_t *pdc)
{
  ASSERT(pdc != NULL);

  return pdc->next_buff != NULL;
}
