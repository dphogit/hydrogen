#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gc.h"
#include "memory.h"
#include "object.h"
#include "table.h"

// Helper macro so consumer of allocateObject does not have to cast.
#define ALLOCATE_OBJ(gc, type, objectType)                                     \
  (type *)allocateObject(gc, sizeof(type), objectType)

static Obj *allocateObject(GC *gc, size_t size, ObjType type) {
  Obj *object = reallocate(NULL, 0, size);
  object->type = type;
  gcAddObject(gc, object);
  return object;
}

static ObjString *allocateString(GC *gc, Table *strings, char *chars, int n,
                                 uint32_t hash) {
  ObjString *string = ALLOCATE_OBJ(gc, ObjString, OBJ_STRING);
  string->length = n;
  string->chars = chars;
  string->hash = hash;
  tableSet(strings, string, NIL_VAL); // Using the table as a hash set.
  return string;
}

// FNV-1a hash function (32-bit)
static uint32_t hashString(const char *key, int n) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < n; i++) {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}

ObjString *takeString(GC *gc, Table *strings, char *chars, int n) {
  uint32_t hash = hashString(chars, n);

  ObjString *interned = tableFindString(strings, chars, n, hash);
  if (interned != NULL) {
    FREE_ARRAY(char, chars, n + 1);
    return interned;
  }

  return allocateString(gc, strings, chars, n, hash);
}

ObjString *copyString(GC *gc, Table *strings, const char *chars, int n) {
  uint32_t hash = hashString(chars, n);

  ObjString *interned = tableFindString(strings, chars, n, hash);
  if (interned != NULL)
    return interned;

  char *buffer = ALLOCATE(char, n + 1);
  memcpy(buffer, chars, n);
  buffer[n] = '\0';
  return allocateString(gc, strings, buffer, n, hash);
}

void printObject(Value value) {
  switch (AS_OBJ(value)->type) {
  case OBJ_STRING:
    printf("%s", AS_CSTRING(value));
    break;
  }
}
