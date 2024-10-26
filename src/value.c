#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"

void initValueArray(ValueArray *arr) {
  arr->count = 0;
  arr->capacity = 0;
  arr->values = NULL;
}

static void growValueArray(ValueArray *arr) {
  int oldCapacity = arr->capacity;
  int newCapacity = GROW_CAPACITY(oldCapacity);

  arr->capacity = newCapacity;
  arr->values = GROW_ARRAY(Value, arr->values, oldCapacity, newCapacity);
}

void appendValueArray(ValueArray *arr, Value value) {
  if (arr->count + 1 > arr->capacity) {
    growValueArray(arr);
  }
  arr->values[arr->count] = value;
  arr->count++;
}

void freeValueArray(ValueArray *arr) {
  FREE_ARRAY(Value, arr->values, arr->capacity);
  initValueArray(arr);
}

bool valuesEqual(Value a, Value b) {
  if (a.type != b.type)
    return false;

  switch (a.type) {
  case VAL_NUMBER:
    return AS_NUMBER(a) == AS_NUMBER(b);
  case VAL_BOOL:
    return AS_BOOL(a) == AS_BOOL(b);
  case VAL_NIL:
    return true;
  case VAL_OBJ: {
    ObjString *s1 = AS_STRING(a), *s2 = AS_STRING(b);
    return s1->length == s2->length && memcmp(s1, s2, s1->length) == 0;
  }
  default:
    return false; // Unreachable.
  }
}

void printValue(Value value) {
  switch (value.type) {
  case VAL_NUMBER:
    printf("%g", AS_NUMBER(value));
    break;
  case VAL_BOOL:
    printf(AS_BOOL(value) ? "true" : "false");
    break;
  case VAL_NIL:
    printf("nil");
    break;
  case VAL_OBJ:
    printObject(value);
    break;
  }
}
