#ifndef HYDRO_CHUNK_H
#define HYDRO_CHUNK_H

#include <stdint.h>

#include "value.h"

/*
 * Implements a dynamic array of bytecode instructions, consisting of a
 * constants array to store data associated with the bytecode instructions.
 *
 * In addition to the array of instructions, there is an array corresponding
 * of the line number for the bytecode instruction.
 */
typedef struct Chunk {
  int count;
  int capacity;
  uint8_t *code;
  ValueArray constants;
  int *lines;
} Chunk;

void initChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte, int line);
void freeChunk(Chunk *chunk);
int addConstant(Chunk *chunk, Value value);

typedef enum OpCode {
  OP_CONSTANT,
  OP_POP,
  OP_PUSH,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NEGATE,
  OP_RETURN
} OpCode;

#endif
