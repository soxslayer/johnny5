#pragma once

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
