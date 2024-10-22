#include "utest.h"
#include "vm.h"

UTEST(VM, stackOperations) {
  VM vm;
  initVM(&vm);

  push(&vm, 69.0);
  ASSERT_EQ(*(vm.stackTop - 1), 69.0);

  Value popped = pop(&vm);
  ASSERT_EQ(popped, 69.0);

  freeVM(&vm);
}

UTEST(VM, expressions) {
  VM vm;
  initVM(&vm);

  char *expressions[] = {"3 + 8", "9 - 4", "5 * 8", "10 / 10", "-(5 - 2)"};

  for (int i = 0; i < 5; i++) {
    InterpretResult result = interpret(expressions[i]);
    EXPECT_EQ(result, (InterpretResult)INTERPRET_OK);
  }

  freeVM(&vm);
}
