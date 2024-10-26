#include <stdbool.h>
#include <stdio.h>

#include "chunk.h"
#include "compiler.h"
#include "gc.h"
#include "object.h"
#include "utest.h"

struct CompilerTestFixture {
  Chunk chunk;
  GC gc;
};

UTEST_F_SETUP(CompilerTestFixture) {
  initChunk(&utest_fixture->chunk);
  initGC(&utest_fixture->gc);
  ASSERT_TRUE(1);
}

UTEST_F_TEARDOWN(CompilerTestFixture) {
  freeChunk(&utest_fixture->chunk);
  freeGC(&utest_fixture->gc);
  ASSERT_TRUE(1);
}

#define OpCodeTest(source, opcode)                                             \
  Chunk chunk = utest_fixture->chunk;                                          \
                                                                               \
  bool result = compile(source, &chunk, &utest_fixture->gc);                   \
                                                                               \
  ASSERT_TRUE(result);                                                         \
  ASSERT_EQ(chunk.count, 2);                                                   \
                                                                               \
  ASSERT_EQ(chunk.code[0], opcode);                                            \
  ASSERT_EQ(chunk.code[1], OP_RETURN);

UTEST_F(CompilerTestFixture, compileTrue) { OpCodeTest("true", OP_TRUE); }

UTEST_F(CompilerTestFixture, compileFalse) { OpCodeTest("false", OP_FALSE); }

UTEST_F(CompilerTestFixture, compileNil) { OpCodeTest("nil", OP_NIL); }

UTEST_F(CompilerTestFixture, compileBang) {
  Chunk chunk = utest_fixture->chunk;

  bool result = compile("!true", &chunk, &utest_fixture->gc);

  ASSERT_TRUE(result);

  ASSERT_EQ(chunk.count, 3);

  ASSERT_EQ(chunk.code[0], OP_TRUE);
  ASSERT_EQ(chunk.code[1], OP_NOT);
  ASSERT_EQ(chunk.code[2], OP_RETURN);
}

UTEST_F(CompilerTestFixture, compileNumber) {
  Chunk chunk = utest_fixture->chunk;

  bool result = compile("69", &chunk, &utest_fixture->gc);

  ASSERT_TRUE(result);

  ASSERT_EQ(chunk.count, 3);

  ASSERT_EQ(chunk.code[0], OP_CONSTANT);
  ASSERT_EQ(chunk.code[1], 0);
  ASSERT_EQ(chunk.code[2], OP_RETURN);

  ASSERT_EQ(chunk.constants.count, 1);
  ASSERT_EQ(AS_NUMBER(chunk.constants.values[0]), 69);

  for (int i = 0; i < chunk.count; i++) {
    ASSERT_EQ(chunk.lines[i], 1);
  }
}

UTEST_F(CompilerTestFixture, compileNegation) {
  Chunk chunk = utest_fixture->chunk;

  bool result = compile("-69", &chunk, &utest_fixture->gc);

  ASSERT_TRUE(result);

  ASSERT_EQ(chunk.count, 4);

  ASSERT_EQ(chunk.code[0], OP_CONSTANT);
  ASSERT_EQ(chunk.code[1], 0);
  ASSERT_EQ(chunk.code[2], OP_NEGATE);
  ASSERT_EQ(chunk.code[3], OP_RETURN);

  ASSERT_EQ(chunk.constants.count, 1);
  ASSERT_EQ(AS_NUMBER(chunk.constants.values[0]), 69);

  for (int i = 0; i < chunk.count; i++) {
    ASSERT_EQ(chunk.lines[i], 1);
  }
}

UTEST_F(CompilerTestFixture, compileGrouped) {
  Chunk chunk = utest_fixture->chunk;

  bool result = compile("(69)", &chunk, &utest_fixture->gc);

  ASSERT_TRUE(result);

  ASSERT_EQ(chunk.count, 3);

  ASSERT_EQ(chunk.code[0], OP_CONSTANT);
  ASSERT_EQ(chunk.code[1], 0);
  ASSERT_EQ(chunk.code[2], OP_RETURN);

  ASSERT_EQ(chunk.constants.count, 1);
  ASSERT_EQ(AS_NUMBER(chunk.constants.values[0]), 69);

  for (int i = 0; i < chunk.count; i++) {
    ASSERT_EQ(chunk.lines[i], 1);
  }
}

#define BinaryExpressionTest(operator, opcode)                                 \
  Chunk chunk = utest_fixture->chunk;                                          \
  char source[20];                                                             \
  sprintf(source, "69 %s 420", operator);                                      \
  source[19] = '\0';                                                           \
                                                                               \
  bool result = compile(source, &chunk, &utest_fixture->gc);                   \
                                                                               \
  ASSERT_TRUE(result);                                                         \
  ASSERT_EQ(chunk.count, 6);                                                   \
                                                                               \
  ASSERT_EQ(chunk.code[0], OP_CONSTANT);                                       \
  ASSERT_EQ(chunk.code[1], 0);                                                 \
  ASSERT_EQ(chunk.code[2], OP_CONSTANT);                                       \
  ASSERT_EQ(chunk.code[3], 1);                                                 \
  ASSERT_EQ(chunk.code[4], opcode);                                            \
  ASSERT_EQ(chunk.code[5], OP_RETURN);                                         \
                                                                               \
  ASSERT_EQ(chunk.constants.count, 2);                                         \
  ASSERT_EQ(AS_NUMBER(chunk.constants.values[0]), 69);                         \
  ASSERT_EQ(AS_NUMBER(chunk.constants.values[1]), 420);                        \
                                                                               \
  for (int i = 0; i < chunk.count; i++) {                                      \
    ASSERT_EQ(chunk.lines[i], 1);                                              \
  }

UTEST_F(CompilerTestFixture, compilePlus) { BinaryExpressionTest("+", OP_ADD); }

UTEST_F(CompilerTestFixture, compileMinus) {
  BinaryExpressionTest("-", OP_SUBTRACT);
}

UTEST_F(CompilerTestFixture, compileMultiply) {
  BinaryExpressionTest("*", OP_MULTIPLY);
}

UTEST_F(CompilerTestFixture, compileDivide) {
  BinaryExpressionTest("/", OP_DIVIDE);
}

UTEST_F(CompilerTestFixture, compileLessThan) {
  BinaryExpressionTest("<", OP_LT);
}

UTEST_F(CompilerTestFixture, compileLessEqual) {
  BinaryExpressionTest("<=", OP_LTE);
}

UTEST_F(CompilerTestFixture, compileEqual) {
  BinaryExpressionTest("==", OP_EQ);
}

UTEST_F(CompilerTestFixture, compileNotEqual) {
  BinaryExpressionTest("!=", OP_NEQ);
}

UTEST_F(CompilerTestFixture, compileGreater) {
  BinaryExpressionTest(">", OP_GT);
}

UTEST_F(CompilerTestFixture, compileGreaterEqual) {
  BinaryExpressionTest(">=", OP_GTE);
}

UTEST_F(CompilerTestFixture, compileString) {
  Chunk chunk = utest_fixture->chunk;

  bool result = compile("\"Hello, World!\"", &chunk, &utest_fixture->gc);

  ASSERT_TRUE(result);

  ASSERT_EQ(chunk.count, 3);

  ASSERT_EQ(chunk.code[0], OP_CONSTANT);
  ASSERT_EQ(chunk.code[1], 0);
  ASSERT_EQ(chunk.code[2], OP_RETURN);

  ASSERT_EQ(chunk.constants.count, 1);
  ASSERT_TRUE(IS_STRING(chunk.constants.values[0]));
  ASSERT_STREQ(AS_CSTRING(chunk.constants.values[0]), "Hello, World!");
}
