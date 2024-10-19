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

  appendValueArray(&valueArray, 6.9);

  ASSERT_EQ(valueArray.count, 1);
  ASSERT_EQ(valueArray.values[0], 6.9);
}
