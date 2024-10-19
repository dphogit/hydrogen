#ifndef HYDRO_CHUNK_H
#define HYDRO_CHUNK_H

#include <stdint.h>

#include "value.h"

/*
 * Implements a dynamic array of bytecode instructions, consisting of a
 * constants array to store data associated with the bytecode instructions.
 */
typedef struct Chunk {
  int count;
  int capacity;
  uint8_t *code;
  ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte);
void freeChunk(Chunk *chunk);
int addConstant(Chunk *chunk, Value value);

#endif
