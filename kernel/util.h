#pragma once

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))

#define MIN(x, y) \
({ \
  typeof(x) __x = (x); \
  typeof(y) __y = (y); \
  __x < __y ? __x : __y; \
})

#define MAX(x, y) \
({ \
  typeof(x) __x = (x); \
  typeof(y) __y = (y); \
  __x > __y ? __x : __y; \
})

#define ASSIGN_MIN(x, y) (x) = MIN(x, y)
#define ASSIGN_MAX(x, y) (x) = MAX(x, y)

#define LAMBDA(ret, ...) ({ ret __fn__ __VA_ARGS__ __fn__; })

#ifdef TEST
# include "basic_uart.h"

# define UT_HEADER() ({ \
    basic_uart_tx_str("--- "); \
    basic_uart_tx_str(__func__); \
    basic_uart_tx_str(" ---\r\n"); \
  })

# define UT_FOOTER() ({ \
    basic_uart_tx_str("--- "); \
    basic_uart_tx_str(__func__); \
    basic_uart_tx_str(" passed ---\r\n"); \
  })
#endif
