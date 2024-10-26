#ifndef HYDRO_VALUE_H
#define HYDRO_VALUE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Obj Obj;
typedef struct ObjString ObjString;

typedef enum ValueType {
  VAL_BOOL,
  VAL_NIL,
  VAL_NUMBER,
  VAL_OBJ,
} ValueType;

typedef struct Value {
  ValueType type;
  union as {
    bool boolean;
    double number;
    Obj *obj;
  } as;
} Value;

// Macros to hoist statically typed values into the languages dynamic types
#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})
#define OBJ_VAL(object) ((Value){VAL_OBJ, {.obj = (Obj *)object}})

// Macros to unpack dynamic types to statically typed values
#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)
#define AS_OBJ(value) ((value).as.obj)

// Macros to perform checking before unpacking dynamic types
#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)
#define IS_OBJ(value) ((value).type == VAL_OBJ)

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
