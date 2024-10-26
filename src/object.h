#ifndef HYDRO_OBJECT_H
#define HYDRO_OBJECT_H

#include "value.h"

#define OBJ_TYPE(value) (AS_OBJ(value)->type)

#define IS_STRING(value) isObjType(value, OBJ_STRING)

#define AS_STRING(value) ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString *)AS_OBJ(value))->chars)

typedef enum ObjType {
  OBJ_STRING,
} ObjType;

struct Obj {
  ObjType type;
};

struct ObjString {
  Obj obj;
  int length;
  char *chars; // Future could look to use the flexible array members techinique
               // for efficiency to reduce a pointer indirection.
};

/* Directly takes ownership and creates a from the same memory that chars
 * is using. Use copyString instead if creating a copy is desired. */
ObjString *takeString(char *chars, int length);
ObjString *copyString(const char *chars, int length);

void printObject(Value value);

static inline bool isObjType(Value value, ObjType type) {
  return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif