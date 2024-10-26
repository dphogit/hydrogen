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

// TODO: We keep these tests simple while implementation is ongoing.
// Also can't assert output as it is only being printed currently.
UTEST(VM, expressions) {
  VM vm;
  initVM(&vm);

  char *expressions[] = {
      "3 + 8",    "9 - 4", "5 * 8",  "10 / 10",
      "-(5 - 2)", "false", "true",   "nil",
      "!true",    "1 < 2", "1 <= 2", "1 == 2",
      "1 != 2",   "1 > 2", "1 >= 2", "\"Hello\" + \", World!\""};

  int n = sizeof(expressions) / sizeof(expressions[0]);
  for (int i = 0; i < n; i++) {
    printf("%s => ", expressions[i]);
    InterpretResult result = interpret(expressions[i]);
    EXPECT_EQ(result, (InterpretResult)INTERPRET_OK);
  }

  freeVM(&vm);
}
