#include "utest.h"
#include "vm.h"

struct VMTestFixture {
  VM vm;
};

UTEST_F_SETUP(VMTestFixture) {
  initVM(&utest_fixture->vm);
  ASSERT_TRUE(1);
}

UTEST_F_TEARDOWN(VMTestFixture) {
  freeVM(&utest_fixture->vm);
  ASSERT_TRUE(1);
}

UTEST_F(VMTestFixture, stackOperations) {
  push(&utest_fixture->vm, NUMBER_VAL(69.0));
  ASSERT_EQ(AS_NUMBER(*(utest_fixture->vm.stackTop - 1)), 69.0);

  Value popped = pop(&utest_fixture->vm);
  ASSERT_EQ(AS_NUMBER(popped), 69.0);
}

UTEST_F(VMTestFixture, expressionStatements) {
  char *expressionStmts[] = {
      "3 + 8;",    "9 - 4;", "5 * 8;",  "10 / 10;",
      "-(5 - 2);", "false;", "true;",   "nil;",
      "!true;",    "1 < 2;", "1 <= 2;", "1 == 2;",
      "1 != 2;",   "1 > 2;", "1 >= 2;", "\"Hello\" + \", World!\";"};

  int n = sizeof(expressionStmts) / sizeof(expressionStmts[0]);
  for (int i = 0; i < n; i++) {
    InterpretResult result = interpret(&utest_fixture->vm, expressionStmts[i]);
    EXPECT_EQ(result, (InterpretResult)INTERPRET_OK);
  }
}

// Assert one instance of VM across interpret calls runs ok.
// Example scenario would be a REPL environment where var is declared in one
// interpret call, then another interpret call uses the declared variable.
UTEST_F(VMTestFixture, globalVariable) {
  InterpretResult result = interpret(&utest_fixture->vm, "var x = 6.9;");
  ASSERT_EQ(result, (InterpretResult)INTERPRET_OK);

  result = interpret(&utest_fixture->vm, "x;");
  ASSERT_EQ(result, (InterpretResult)INTERPRET_OK);
}

UTEST_F(VMTestFixture, localVariable) {
  InterpretResult result = interpret(&utest_fixture->vm, "{ var x = 6.9; x; }");
  ASSERT_EQ(result, (InterpretResult)INTERPRET_OK);
}
