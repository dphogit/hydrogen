#include <stddef.h>
#include <stdlib.h>

#include "chunk.h"
#include "value.h"

void initChunk(Chunk *chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  initValueArray(&chunk->constants);
}

// Grows the chunk's capacity and memory to allow for more instructions.
static void growChunk(Chunk *chunk) {
  chunk->capacity = chunk->capacity < 8 ? 8 : chunk->capacity * 2;
  chunk->code = realloc(chunk->code, chunk->capacity);
  if (chunk->code == NULL)
    exit(EXIT_FAILURE);
}

void writeChunk(Chunk *chunk, uint8_t byte) {
  if (chunk->count + 1 > chunk->capacity) {
    growChunk(chunk);
  }
  chunk->code[chunk->count] = byte;
  chunk->count++;
}

// Returns the index of the inserted value in the chunk's constants array.
int addConstant(Chunk *chunk, Value value) {
  appendValueArray(&chunk->constants, value);
  return chunk->constants.count - 1;
}

void freeChunk(Chunk *chunk) {
  freeValueArray(&chunk->constants);
  free(chunk->code);
  initChunk(chunk);
}
