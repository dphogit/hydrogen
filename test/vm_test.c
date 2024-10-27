#include "utest.h"
#include "vm.h"

UTEST(VM, stackOperations) {
  VM vm;
  initVM(&vm);

  push(&vm, NUMBER_VAL(69.0));
  ASSERT_EQ(AS_NUMBER(*(vm.stackTop - 1)), 69.0);

  Value popped = pop(&vm);
  ASSERT_EQ(AS_NUMBER(popped), 69.0);

  freeVM(&vm);
}

UTEST(VM, expressionStatements) {
  VM vm;
  initVM(&vm);

  char *expressionStmts[] = {
      "3 + 8;",    "9 - 4;", "5 * 8;",  "10 / 10;",
      "-(5 - 2);", "false;", "true;",   "nil;",
      "!true;",    "1 < 2;", "1 <= 2;", "1 == 2;",
      "1 != 2;",   "1 > 2;", "1 >= 2;", "\"Hello\" + \", World!\";"};

  int n = sizeof(expressionStmts) / sizeof(expressionStmts[0]);
  for (int i = 0; i < n; i++) {
    InterpretResult result = interpret(expressionStmts[i]);
    EXPECT_EQ(result, (InterpretResult)INTERPRET_OK);
  }

  freeVM(&vm);
}
