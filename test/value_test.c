#include "object.h"
#include "utest.h"
#include "value.h"

struct ValueTestFixture {
  ValueArray valueArray;
};

UTEST_F_SETUP(ValueTestFixture) {
  initValueArray(&utest_fixture->valueArray);
  ASSERT_TRUE(1);
}

UTEST_F_TEARDOWN(ValueTestFixture) {
  freeValueArray(&utest_fixture->valueArray);
  ASSERT_TRUE(1);
}

UTEST_F(ValueTestFixture, initValueArray) {
  ValueArray valueArray = utest_fixture->valueArray;

  ASSERT_EQ(valueArray.count, 0);
  ASSERT_EQ(valueArray.capacity, 0);
  ASSERT_EQ(valueArray.values, NULL);
}

UTEST_F(ValueTestFixture, appendValueArray) {
  ValueArray valueArray = utest_fixture->valueArray;

  appendValueArray(&valueArray, NUMBER_VAL(6.9));

  ASSERT_EQ(valueArray.count, 1);
  ASSERT_EQ(AS_NUMBER(valueArray.values[0]), 6.9);
}

UTEST(Value, valuesEqual) {
  GC gc;
  initGC(&gc);
  Table strings;
  initTable(&strings);

  Value a = BOOL_VAL(true), b = BOOL_VAL(true);
  EXPECT_TRUE(valuesEqual(a, b));

  a = BOOL_VAL(true), b = BOOL_VAL(false);
  EXPECT_FALSE(valuesEqual(a, b));

  a = NUMBER_VAL(6.9), b = NUMBER_VAL(6.9);
  EXPECT_TRUE(valuesEqual(a, b));

  a = NUMBER_VAL(6.9), b = NUMBER_VAL(4.2);
  EXPECT_FALSE(valuesEqual(a, b));

  a = NIL_VAL, b = NIL_VAL;
  EXPECT_TRUE(valuesEqual(a, b));

  // Different value types.
  a = NIL_VAL, b = BOOL_VAL(false);
  EXPECT_FALSE(valuesEqual(a, b));

  // Strings (different objects on heap, but we expect value equality)
  a = OBJ_VAL(copyString(&gc, &strings, "Hello", 5));
  b = OBJ_VAL(copyString(&gc, &strings, "Hello", 5));
  EXPECT_TRUE(valuesEqual(a, b));

  freeGC(&gc);
  freeTable(&strings);
}
