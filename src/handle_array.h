#pragma once

#include "limits.h"
#include "types.h"

#define INVALID_HANDLE U32_MAX

typedef struct _handle_array_t
{
  void **handles;
  size_t handles_size;
  u32 *free_handles;
} handle_array_t;

void handle_array_create(handle_array_t *array, size_t size);
void handle_array_destroy(handle_array_t *array);
u32 handle_array_alloc(handle_array_t *array, void *item);
void handle_array_free(handle_array_t *array, u32 handle);
void *handle_array_get(handle_array_t *array, u32 handle);
