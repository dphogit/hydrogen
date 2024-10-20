#include <stdio.h>
#include <stdlib.h>

#include "value.h"

void initValueArray(ValueArray *arr) {
  arr->count = 0;
  arr->capacity = 0;
  arr->values = NULL;
}

static void growValueArray(ValueArray *arr) {
  arr->capacity = arr->capacity < 8 ? 8 : arr->capacity * 2;
  arr->values = realloc(arr->values, arr->capacity);
  if (arr->values == NULL)
    exit(EXIT_FAILURE);
}

void appendValueArray(ValueArray *arr, Value value) {
  if (arr->count + 1 > arr->capacity) {
    growValueArray(arr);
  }
  arr->values[arr->count] = value;
  arr->count++;
}

void freeValueArray(ValueArray *arr) {
  free(arr->values);
  initValueArray(arr);
}

void printValue(Value value) { printf("%g", value); }
