#include "uart.h"

#include "assert.h"
#include "bits.h"
#include "clock.h"
#include "event.h"
#include "nvic.h"
#include "task.h"
#include "util.h"

#define CDEV_TO_UART(x) containerof(x, uart_t, cdev)

static void uart_set_baud(uart_t *uart, u32 baud)
{
  uart->regs->brgr = (clock_speed / (baud * 16)) & 0xffff;
}

static void uart_set_parity(uart_t *uart, u32 parity)
{
  set_field(uart->regs->mr, _bm(parity & 0x7, 9), _bm(0x7, 9));
}

static void uart_set_stop(uart_t *uart, u32 stop)
{
  set_field(uart->regs->mr, _bm(stop & 0x3, 12), _bm(0x7, 12));
}

static void uart_set_channel_mode(uart_t *uart, u32 mode)
{
  set_field(uart->regs->mr, _bm(mode & 0x3, 14), _bm(0x7, 14));
}

static void uart_set_charlen(uart_t *uart, u32 charlen)
{
  if (charlen == IOCTL_9_BIT_CHARLEN)
    set_bits(uart->regs->mr, _b(17));
  else {
    clr_bits(uart->regs->mr, _b(17));
    set_field(uart->regs->mr, _bm(charlen & 0x3, 6), _bm(0x7, 6));
  }
}

static void uart_flush_write_buff(uart_t *uart)
{
  if (uart->write_buff != NULL) {
    pdc_queue_buff(&uart->pdc_tx, uart->write_buff);
    uart->write_buff = NULL;
    set_bits(uart->regs->ier, _b(4));
    pdc_enable(&uart->pdc_tx);
  }
}

static buff_t * uart_take_buff(uart_t *uart, bool block)
{
  if (!sem_try_take(&uart->free_buffers)) {
    if (!block)
      return NULL;

    spinlock_release(&uart->cdev.lock);
    sem_take(&uart->free_buffers);
    spinlock_acquire(&uart->cdev.lock);
  }

  int idx = find_clr(uart->alloc_buffers);
  set_bits(uart->alloc_buffers, _b(idx));

  return &uart->buffers[idx];
}

static void uart_give_buff(uart_t *uart, buff_t *buff)
{
  int b = buff - uart->buffers;

  clr_bits(uart->alloc_buffers, _b(b));
  sem_give(&uart->free_buffers);
}

static void uart_queue_rx_buffs(uart_t *uart)
{
  while (!pdc_next_buff_queued(&uart->pdc_rx)) {
    buff_t *b = uart_take_buff(uart, false);

    if (b == NULL)
      break;

    buff_reset(b);
    pdc_queue_buff(&uart->pdc_rx, b);
  }
}

static void uart_enable_tx_int(uart_t *uart)
{
  set_bits(uart->regs->ier, _b(4));
}

static void uart_disable_tx_int(uart_t *uart)
{
  set_bits(uart->regs->idr, _b(4));
}

static bool uart_tx_int_enabled(uart_t *uart)
{
  return bits_set(uart->regs->imr, _b(4)) > 0;
}

static void uart_enable_rx_int(uart_t *uart)
{
  set_bits(uart->regs->ier, _b(3));
}

static void uart_disable_rx_int(uart_t *uart)
{
  set_bits(uart->regs->idr, _b(3));
}

static bool uart_rx_int_enabled(uart_t *uart)
{
  return bits_set(uart->regs->imr, _b(3)) > 0;
}

static int uart_open(chardev_t *cdev)
{
  uart_t *uart = CDEV_TO_UART(cdev);

  peripheral_enable_clock(uart->id);

  uart->regs->idr = 0xffffffff;
  uart->regs->cr = _b(2) | _b(3) | _b(8);

  uart->regs->cr = _b(4) | _b(6);

  pdc_reset(&uart->pdc_tx);
  pdc_reset(&uart->pdc_rx);

  spinlock_acquire(&cdev->lock);

  uart_queue_rx_buffs(uart);
  uart->read_buff = NULL;
  uart->write_buff = uart_take_buff(uart, true);

  spinlock_release(&cdev->lock);

  uart_enable_rx_int(uart);
  pdc_enable(&uart->pdc_rx);
  peripheral_enable_int(uart->id);

  u32 tx_mask = _b(uart->pio_tx);
  u32 rx_mask = _b(uart->pio_rx);

  uart->pio_regs->pdr = tx_mask | rx_mask;
  uart->pio_regs->puer = tx_mask;

  return 0;
}

static int uart_close(chardev_t *cdev)
{
  uart_t *uart = CDEV_TO_UART(cdev);

  peripheral_disable_int(uart->id);

  uart->regs->idr = 0xffffffff;
  clr_bits(uart->regs->cr, _b(5) | _b(7));

  pdc_reset(&uart->pdc_tx);
  pdc_reset(&uart->pdc_rx);

  pdc_disable(&uart->pdc_tx);
  pdc_disable(&uart->pdc_rx);

  int b;
  while ((b = find_set(uart->alloc_buffers)) < NELEMS(uart->buffers))
    uart_give_buff(uart, &uart->buffers[b]);

  peripheral_disable_clock(uart->id);

  return 0;
}

static ssize_t uart_read(chardev_t *cdev, void *dst, size_t size)
{
  uart_t *uart = CDEV_TO_UART(cdev);
  ssize_t r = 0;

  spinlock_acquire(&uart->cdev.lock);

  if (uart->read_buff == NULL) {
    pdc_swap_buff(&uart->pdc_rx);
    uart->read_buff = pdc_dequeue_buff(&uart->pdc_rx);
  }

  if (uart->read_buff != NULL) {
    r = buff_read(uart->read_buff, dst, size);

    if (buff_empty(uart->read_buff)) {
      uart_give_buff(uart, uart->read_buff);
      uart->read_buff = NULL;
    }
  }

  uart_queue_rx_buffs(uart);

  spinlock_release(&uart->cdev.lock);

  return r;
}

static ssize_t uart_write(chardev_t *cdev, const void *src, size_t size)
{
  uart_t *uart = CDEV_TO_UART(cdev);
  ssize_t r = 0;

  spinlock_acquire(&uart->cdev.lock);

  if (uart->write_buff == NULL)
    uart->write_buff = uart_take_buff(uart, true);

  r = buff_write(uart->write_buff, src, size);

  if (buff_full(uart->write_buff))
    uart_flush_write_buff(uart);

  spinlock_release(&uart->cdev.lock);

  return r;
}

static size_t uart_poll(chardev_t *cdev)
{
  return 0;
}

static int uart_flush(chardev_t *cdev)
{
  ASSERT(cdev != NULL);

  uart_t *uart = CDEV_TO_UART(cdev);

  spinlock_acquire(&cdev->lock);

  if (uart->write_buff != NULL)
    uart_flush_write_buff(uart);

  spinlock_release(&cdev->lock);

  return 0;
}

static int uart_ioctl(chardev_t *cdev, u32 ioop, void *param)
{
  ASSERT(cdev != NULL);

  uart_t *uart = CDEV_TO_UART(cdev);
  int r = -1;

  spinlock_acquire(&cdev->lock);

  switch (ioop) {
    case IOCTL_SETBAUD:
      ASSERT(param != NULL);

      uart_set_baud(uart, *((u32 *)param));
      r = 0;

      break;

    case IOCTL_SETPARITY:
      ASSERT(param != NULL);

      uart_set_parity(uart, *((u32 *)param));
      r = 0;

      break;

    case IOCTL_SETSTOP:
      ASSERT(param != NULL);

      uart_set_stop(uart, *((u32 *)param));
      r = 0;

      break;

    case IOCTL_SETCHMODE:
      ASSERT(param != NULL);

      uart_set_channel_mode(uart, *((u32 *)param));
      r = 0;

      break;

    case IOCTL_SETCHARLEN:
      ASSERT(param != NULL);

      uart_set_charlen(uart, *((u32 *)param));
      r = 0;

      break;

    default:
      break;
  }

  spinlock_release(&cdev->lock);

  return r;
}

static ISR void uart0_isr();
static ISR void usart0_isr();
static ISR void usart1_isr();
static ISR void usart2_isr();
static ISR void usart3_isr();

static uart_t __uart_devs[] = {
  {
    .cdev = {
      .open = uart_open,
      .close = uart_close,
      .read = uart_read,
      .write = uart_write,
      .poll = uart_poll,
      .flush = uart_flush,
      .ioctl = uart_ioctl,
      .name = "uart0"
    },
    .usart = false,
    .id = UART_ID,
    .irq_handler = uart0_isr,
    .regs = &Uart0
  },
  {
    .cdev = {
      .open = uart_open,
      .close = uart_close,
      .read = uart_read,
      .write = uart_write,
      .poll = uart_poll,
      .flush = uart_flush,
      .ioctl = uart_ioctl,
      .name = "usart0"
    },
    .usart = false,
    .id = USART0_ID,
    .irq_handler = usart0_isr,
    .regs = &Usart0,
    .pio_regs = &PioA,
    .pio_tx = 11,
    .pio_rx = 10,
    .pio_mode = 0
  }
};

event_queue_t __isr_event_q;

static void isr_handler(uart_t *uart)
{
  if (bits_set(uart->regs->sr, _b(4)) && uart_tx_int_enabled(uart)) {
    pdc_swap_buff(&uart->pdc_tx);
    event_queue_signal(&__isr_event_q, uart->tx_event);
    if (!pdc_cur_buff_queued(&uart->pdc_tx))
      uart_disable_tx_int(uart);
  }
  if (bits_set(uart->regs->sr, _b(3)) && uart_rx_int_enabled(uart)) {
    pdc_swap_buff(&uart->pdc_rx);
    event_queue_signal(&__isr_event_q, uart->rx_event);
    uart_disable_rx_int(uart);
  }
}

static ISR void uart0_isr()
{
  isr_handler(&__uart_devs[0]);
}

static ISR void usart0_isr()
{
  isr_handler(&__uart_devs[1]);
}

static ISR void usart1_isr()
{
  isr_handler(&__uart_devs[2]);
}

static ISR void usart2_isr()
{
  isr_handler(&__uart_devs[3]);
}

static ISR void usart3_isr()
{
  isr_handler(&__uart_devs[4]);
}

static void uart_dev_init(uart_t *uart)
{
  chardev_register(&uart->cdev);
  pdc_init(&uart->pdc_tx, &uart->regs->pdc, PDC_TX);
  pdc_init(&uart->pdc_rx, &uart->regs->pdc, PDC_RX);
  nvic_set_handler(IRQ_VECTOR(uart->id), uart->irq_handler);
  uart->rx_event = event_queue_create_event(&__isr_event_q);
  uart->tx_event = event_queue_create_event(&__isr_event_q);
  uart->alloc_buffers = 0;
  for (int i = 0; i < NELEMS(uart->buffers); ++i)
    buff_create(&uart->buffers[i], BUFF_SIZE);
  sem_init(&uart->free_buffers, NELEMS(uart->buffers), NELEMS(uart->buffers));
  uart->read_buff = NULL;
  uart->write_buff = NULL;
}

static void uart_handle_rx_event(uart_t *uart)
{
  spinlock_acquire(&uart->cdev.lock);

  uart_queue_rx_buffs(uart);
  uart_enable_rx_int(uart);

  spinlock_release(&uart->cdev.lock);
}

static void uart_handle_tx_event(uart_t *uart)
{
  spinlock_acquire(&uart->cdev.lock);

  pdc_disable(&uart->pdc_tx);

  buff_t *buff;
  while ((buff = pdc_dequeue_buff(&uart->pdc_tx)) != NULL)
    uart_give_buff(uart, buff);

  if (pdc_cur_buff_queued(&uart->pdc_tx))
    pdc_enable(&uart->pdc_tx);

  spinlock_release(&uart->cdev.lock);
}

static int uart_data_handler()
{
  while (1) {
    event_t *evt = event_queue_wait(&__isr_event_q);

    for (int i = 0; i < NELEMS(__uart_devs); ++i) {
      if (evt == __uart_devs[i].rx_event)
        uart_handle_rx_event(&__uart_devs[i]);
      else if (evt == __uart_devs[i].tx_event)
        uart_handle_tx_event(&__uart_devs[i]);
    }
  }

  return 0;
}

void uart_init()
{
  event_queue_init(&__isr_event_q);

  for (int i = 0; i < NELEMS(__uart_devs); ++i)
    uart_dev_init(&__uart_devs[i]);

  task_add(uart_data_handler, 1024, 0, "uart_isr");
}
