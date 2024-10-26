#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gc.h"
#include "memory.h"
#include "object.h"

// Helper macro so consumer of allocateObject does not have to cast.
#define ALLOCATE_OBJ(gc, type, objectType)                                     \
  (type *)allocateObject(gc, sizeof(type), objectType)

static Obj *allocateObject(GC *gc, size_t size, ObjType type) {
  Obj *object = reallocate(NULL, 0, size);
  if (object == NULL)
    exit(EXIT_FAILURE);

  object->type = type;

  gcAddObject(gc, object);

  return object;
}

static ObjString *allocateString(GC *gc, char *chars, int n) {
  ObjString *string = ALLOCATE_OBJ(gc, ObjString, OBJ_STRING);
  string->length = n;
  string->chars = chars;
  return string;
}

ObjString *takeString(GC *gc, char *chars, int n) {
  return allocateString(gc, chars, n);
}

ObjString *copyString(GC *gc, const char *chars, int n) {
  char *buffer = ALLOCATE(char, n + 1);
  if (buffer == NULL)
    exit(EXIT_FAILURE);

  memcpy(buffer, chars, n);
  buffer[n] = '\0';
  return allocateString(gc, buffer, n);
}

void printObject(Value value) {
  switch (AS_OBJ(value)->type) {
  case OBJ_STRING:
    printf("%s", AS_CSTRING(value));
    break;
  }
}
