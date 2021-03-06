#include "handle_array.h"

#include "assert.h"
#include "bits.h"
#include "mem.h"

void handle_array_create(handle_array_t *array, size_t size)
{
  ASSERT(array != NULL);

  array->handles = malloc(size * sizeof(void *));
  array->handles_size = size;

  u32 num_free = (array->handles_size + 31) / 32;
  array->free_handles = malloc(num_free * sizeof(u32));

  for (u32 i = 0; i < num_free; ++i)
    array->free_handles[i] = 0xffffffff;
}

void handle_array_destroy(handle_array_t *array)
{
  ASSERT(array != NULL);

  free(array->handles);
  free(array->free_handles);
}

u32 handle_array_alloc(handle_array_t *array, void *item)
{
  ASSERT(array != NULL);

  u32 num_free = ((array->handles_size + 31) / 32);

  u32 handle = 0;
  u32 i;
  for (i = 0; i < num_free; ++i) {
    handle = find_set(array->free_handles[i]);
    if (handle == -1)
      continue;

    break;
  }

  if (i == num_free)
    return INVALID_HANDLE;

  handle += i * 32;
  array->handles[handle] = item;
  clr_bits(array->free_handles[i], _b(handle));

  return handle;
}

void handle_array_free(handle_array_t *array, u32 handle)
{
  ASSERT(array != NULL);

  u32 free_idx = handle / 32;
  u32 free_bit = handle % 32;

  ASSERT(handle < array->handles_size
         && bits_clr(array->free_handles[free_idx], _b(free_bit)));

  set_bits(array->free_handles[free_idx], _b(free_bit));
}

void * handle_array_get(handle_array_t *array, u32 handle)
{
  ASSERT(array != NULL);

  u32 free_idx = handle / 32;
  u32 free_bit = handle % 32;

  ASSERT(handle < array->handles_size
         && bits_clr(array->free_handles[free_idx], _b(free_bit)));

  return array->handles[handle];
}
