#ifndef HYDRO_VALUE_H
#define HYDRO_VALUE_H

#include <stddef.h>

typedef double Value;

/* Implements a dynamic array of program data values. */
typedef struct ValueArray {
  int count;
  int capacity;
  Value *values;
} ValueArray;

void initValueArray(ValueArray *arr);
void appendValueArray(ValueArray *arr, Value value);
void freeValueArray(ValueArray *arr);

void printValue(Value value);

#endif
