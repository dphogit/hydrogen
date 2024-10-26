#include "object.h"
#include "utest.h"

UTEST(Object, takeString) {
  char *str = "Hello, World!";

  ObjString *result = takeString(str, 13);

  ASSERT_EQ(result->length, 13);
  ASSERT_EQ(str, result->chars); // Same address - directly took ownership of.
  ASSERT_STREQ(result->chars, "Hello, World!");
  ASSERT_EQ(result->obj.type, (ObjType)OBJ_STRING);
}

UTEST(Object, copyString) {
  char *str = "Hello, World!";

  ObjString *result = copyString(str, 13);

  ASSERT_EQ(result->length, 13);
  ASSERT_NE(str, result->chars); // Not same address (i.e. genuine copy)
  ASSERT_STREQ(result->chars, "Hello, World!");
  ASSERT_EQ(result->obj.type, (ObjType)OBJ_STRING);
}
