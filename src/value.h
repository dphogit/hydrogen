#ifndef HYDRO_VALUE_H
#define HYDRO_VALUE_H

#include <stdbool.h>
#include <stddef.h>

typedef enum ValueType {
  VAL_BOOL,
  VAL_NIL,
  VAL_NUMBER,
} ValueType;

typedef struct Value {
  ValueType type;
  union as {
    bool boolean;
    double number;
  } as;
} Value;

// Macros to hoist statically typed values into the languages dynamic types
#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})

// Macros to unpack dynamic types to statically typed values
#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)

// Macros to perform checking before unpacking dynamic types
#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)

/* Implements a dynamic array of program data values. */
typedef struct ValueArray {
  int count;
  int capacity;
  Value *values;
} ValueArray;

void initValueArray(ValueArray *arr);
void appendValueArray(ValueArray *arr, Value value);
void freeValueArray(ValueArray *arr);

bool valuesEqual(Value a, Value b);

void printValue(Value value);

#endif
