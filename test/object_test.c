#include "object.h"
#include "utest.h"

UTEST(Object, takeString) {
  GC gc;
  initGC(&gc);
  char *str = "Hello, World!";

  ObjString *result = takeString(&gc, str, 13);

  ASSERT_EQ(result->length, 13);
  ASSERT_EQ(str, result->chars); // Same address - directly took ownership of.
  ASSERT_STREQ(result->chars, "Hello, World!");
  ASSERT_EQ(result->obj.type, (ObjType)OBJ_STRING);
}

UTEST(Object, copyString) {
  GC gc;
  initGC(&gc);
  char *str = "Hello, World!";

  ObjString *result = copyString(&gc, str, 13);

  ASSERT_EQ(result->length, 13);
  ASSERT_NE(str, result->chars); // Not same address (i.e. genuine copy)
  ASSERT_STREQ(result->chars, "Hello, World!");
  ASSERT_EQ(result->obj.type, (ObjType)OBJ_STRING);
}
