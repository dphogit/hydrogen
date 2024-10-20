#include <stdbool.h>
#include <stdio.h>

#include "chunk.h"
#include "compiler.h"
#include "utest.h"

struct CompilerTestFixture {
  Chunk chunk;
};

UTEST_F_SETUP(CompilerTestFixture) {
  initChunk(&utest_fixture->chunk);
  ASSERT_TRUE(1);
}

UTEST_F_TEARDOWN(CompilerTestFixture) {
  freeChunk(&utest_fixture->chunk);
  ASSERT_TRUE(1);
}

UTEST_F(CompilerTestFixture, compileNumberLiteral) {
  Chunk chunk = utest_fixture->chunk;

  bool result = compile("69", &chunk);

  ASSERT_TRUE(result);

  ASSERT_EQ(chunk.count, 3);

  ASSERT_EQ(chunk.code[0], OP_CONSTANT);
  ASSERT_EQ(chunk.code[1], 0);
  ASSERT_EQ(chunk.code[2], OP_RETURN);

  ASSERT_EQ(chunk.constants.count, 1);
  ASSERT_EQ(chunk.constants.values[0], 69);

  for (int i = 0; i < chunk.count; i++) {
    ASSERT_EQ(chunk.lines[i], 1);
  }
}

UTEST_F(CompilerTestFixture, compileUnaryNegation) {
  Chunk chunk = utest_fixture->chunk;

  bool result = compile("-69", &chunk);

  ASSERT_TRUE(result);

  ASSERT_EQ(chunk.count, 4);

  ASSERT_EQ(chunk.code[0], OP_CONSTANT);
  ASSERT_EQ(chunk.code[1], 0);
  ASSERT_EQ(chunk.code[2], OP_NEGATE);
  ASSERT_EQ(chunk.code[3], OP_RETURN);

  ASSERT_EQ(chunk.constants.count, 1);
  ASSERT_EQ(chunk.constants.values[0], 69);

  for (int i = 0; i < chunk.count; i++) {
    ASSERT_EQ(chunk.lines[i], 1);
  }
}

UTEST_F(CompilerTestFixture, compileGroupedExpression) {
  Chunk chunk = utest_fixture->chunk;

  bool result = compile("(69)", &chunk);

  ASSERT_TRUE(result);

  ASSERT_EQ(chunk.count, 3);

  ASSERT_EQ(chunk.code[0], OP_CONSTANT);
  ASSERT_EQ(chunk.code[1], 0);
  ASSERT_EQ(chunk.code[2], OP_RETURN);

  ASSERT_EQ(chunk.constants.count, 1);
  ASSERT_EQ(chunk.constants.values[0], 69);

  for (int i = 0; i < chunk.count; i++) {
    ASSERT_EQ(chunk.lines[i], 1);
  }
}

#define BinaryExpressionTest(token, opcode)                                    \
  Chunk chunk = utest_fixture->chunk;                                          \
  char source[9];                                                              \
  sprintf(source, "69 %c 420", token);                                         \
  source[8] = '\0';                                                            \
                                                                               \
  bool result = compile(source, &chunk);                                       \
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
  ASSERT_EQ(chunk.constants.values[0], 69);                                    \
  ASSERT_EQ(chunk.constants.values[1], 420);                                   \
                                                                               \
  for (int i = 0; i < chunk.count; i++) {                                      \
    ASSERT_EQ(chunk.lines[i], 1);                                              \
  }

UTEST_F(CompilerTestFixture, compileBinaryExpressionPlus) {
  BinaryExpressionTest('+', OP_ADD);
}

UTEST_F(CompilerTestFixture, compileBinaryExpressionMinus) {
  BinaryExpressionTest('-', OP_SUBTRACT);
}

UTEST_F(CompilerTestFixture, compiledBinaryExpressionMultiply) {
  BinaryExpressionTest('*', OP_MULTIPLY);
}

UTEST_F(CompilerTestFixture, compiledBinaryExpressionDivider) {
  BinaryExpressionTest('/', OP_DIVIDE);
}
