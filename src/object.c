#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object.h"

// Helper macro so consumer of allocateObject does not have to cast.
#define ALLOCATE_OBJ(type, objectType)                                         \
  (type *)allocateObject(sizeof(type), objectType)

static Obj *allocateObject(size_t size, ObjType type) {
  Obj *object = malloc(size);
  if (object == NULL)
    exit(EXIT_FAILURE);

  object->type = type;
  return object;
}

static ObjString *allocateString(char *chars, int n) {
  ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
  string->length = n;
  string->chars = chars;
  return string;
}

ObjString *takeString(char *chars, int n) { return allocateString(chars, n); }

ObjString *copyString(const char *chars, int n) {
  char *buffer = malloc(sizeof(char) * (n + 1));
  if (buffer == NULL)
    exit(EXIT_FAILURE);

  memcpy(buffer, chars, n);
  buffer[n] = '\0';
  return allocateString(buffer, n);
}

void printObject(Value value) {
  switch (AS_OBJ(value)->type) {
  case OBJ_STRING:
    printf("%s", AS_CSTRING(value));
    break;
  }
}
