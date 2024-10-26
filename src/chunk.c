#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "chunk.h"
#include "memory.h"
#include "value.h"

void initChunk(Chunk *chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  chunk->lines = NULL;
  initValueArray(&chunk->constants);
}

// Grows the chunk's capacity and memory to allow for more instructions.
static void growChunk(Chunk *chunk) {
  int oldCapacity = chunk->capacity;
  int newCapacity = GROW_CAPACITY(oldCapacity);

  chunk->capacity = newCapacity;
  chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, newCapacity);
  chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, newCapacity);
}

void writeChunk(Chunk *chunk, uint8_t byte, int line) {
  if (chunk->count + 1 > chunk->capacity) {
    growChunk(chunk);
  }
  chunk->code[chunk->count] = byte;
  chunk->lines[chunk->count] = line;
  chunk->count++;
}

// Returns the index of the inserted value in the chunk's constants array.
int addConstant(Chunk *chunk, Value value) {
  appendValueArray(&chunk->constants, value);
  return chunk->constants.count - 1;
}

void freeChunk(Chunk *chunk) {
  freeValueArray(&chunk->constants);
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  FREE_ARRAY(int, chunk->lines, chunk->capacity);
  initChunk(chunk);
}
