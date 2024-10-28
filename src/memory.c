#include <stdlib.h>

#include "memory.h"

void *reallocate(void *ptr, __attribute__((unused)) size_t oldSize,
                 size_t newSize) {
  if (newSize == 0) {
    free(ptr);
    return NULL;
  }

  void *result = realloc(ptr, newSize);
  if (result == NULL)
    exit(EXIT_FAILURE);

  return result;
}
