#pragma once

#include "chardev.h"
#include "event.h"
#include "list.h"
#include "nvic.h"
#include "pdc.h"
#include "peripheral.h"
#include "pio.h"
#include "sam3x8e.h"
#include "sem.h"
#include "types.h"

#define NUM_BUFFS 6
#define BUFF_SIZE 32

#define UART_FLAG_RX _b(0)
#define UART_FLAG_TX _b(1)

typedef struct _uart_t
{
  chardev_t cdev;
  bool usart;
  peripheral_id_t id;
  int_handler_t irq_handler;
  uart_regs_t *regs;
  pdc_channel_t pdc_tx;
  pdc_channel_t pdc_rx;
  buff_t buffers[6];
  u32 alloc_buffers;
  sem_t free_buffers;
  buff_t *read_buff;
  buff_t *write_buff;
  pio_regs_t *pio_regs;
  pio_pin_t pio_tx;
  pio_pin_t pio_rx;
  pio_mode_t pio_mode;
  u32 flags;
} uart_t;

/* uart ioctl values */
#define IOCTL_EVEN_PARITY 0
#define IOCTL_ODD_PARITY 1
#define IOCTL_SPACE_PARITY 2
#define IOCTL_MARK_PARITY 3
#define IOCTL_NO_PARITY 4
#define IOCTL_MULTIDROP_PARITY 6

#define IOCTL_1_STOPBIT 0
#define IOCTL_1_5_STOPBIT 1
#define IOCTL_2_STOPBIT 2

#define IOCTL_NORMAL_MODE 0
#define IOCTL_AUTOMATIC_MODE 1
#define IOCTL_LOCAL_LOOPBACK_MODE 2
#define IOCTL_REMOTE_LOOPBACK_MODE 3

#define IOCTL_5_BIT_CHARLEN 0
#define IOCTL_6_BIT_CHARLEN 1
#define IOCTL_7_BIT_CHARLEN 2
#define IOCTL_8_BIT_CHARLEN 3
#define IOCTL_9_BIT_CHARLEN 4

void uart_init();
