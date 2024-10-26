#ifndef HYDRO_MEMORY_H
#define HYDRO_MEMORY_H

#include <stddef.h>

#define ALLOCATE(type, count) (type *)reallocate(NULL, 0, sizeof(type) * count)

#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, ptr, oldCapacity, newCapacity)                        \
  (type *)reallocate(ptr, sizeof(type) * (oldCapacity),                        \
                     sizeof(type) * (newCapacity))

#define FREE_ARRAY(type, ptr, oldCapacity)                                     \
  reallocate(ptr, sizeof(type), oldCapacity)

#define FREE(type, ptr) reallocate(ptr, sizeof(type), 0)

/*
 * The single function used for dynamic memory management in the Hydrogen
 * language (i.e. allocating, freeing and resizing memory). By routing all of
 * these operations through a simple function, data can be tracked (e.g. keeping
 * a running count of the number of bytes of allocated memory) which the garbage
 * collector can use and make it's implementation more easier.
 */
void *reallocate(void *ptr, size_t oldSize, size_t newSize);

#endif
