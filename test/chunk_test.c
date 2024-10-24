#include "chunk.h"
#include "utest.h"
#include "value.h"

struct ChunkTestFixture {
  Chunk chunk;
};

UTEST_F_SETUP(ChunkTestFixture) {
  initChunk(&utest_fixture->chunk);
  ASSERT_TRUE(1);
}

UTEST_F_TEARDOWN(ChunkTestFixture) {
  freeChunk(&utest_fixture->chunk);
  ASSERT_TRUE(1);
}

UTEST_F(ChunkTestFixture, initChunk) {
  Chunk chunk = utest_fixture->chunk;

  ASSERT_EQ(chunk.count, 0);
  ASSERT_EQ(chunk.capacity, 0);
  ASSERT_EQ(chunk.code, NULL);

  ASSERT_EQ(chunk.constants.count, 0);
  ASSERT_EQ(chunk.constants.capacity, 0);
  ASSERT_EQ(chunk.constants.values, NULL);
}

UTEST_F(ChunkTestFixture, writeChunk) {
  Chunk chunk = utest_fixture->chunk;

  writeChunk(&chunk, 69, 1);

  ASSERT_EQ(chunk.code[0], 69);
  ASSERT_EQ(chunk.count, 1);
  ASSERT_EQ(chunk.lines[0], 1);
}

UTEST_F(ChunkTestFixture, addConstant) {
  Chunk chunk = utest_fixture->chunk;

  int constantIndex = addConstant(&chunk, NUMBER_VAL(69.0));

  ASSERT_EQ(constantIndex, 0);
  ASSERT_EQ(chunk.constants.count, 1);
  ASSERT_EQ(AS_NUMBER(chunk.constants.values[0]), 69.0);
}
