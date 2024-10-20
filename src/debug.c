#include <stdint.h>
#include <stdio.h>

#include "chunk.h"
#include "debug.h"

static int simpleInstruction(const char *name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}

static int constantInstruction(const char *name, Chunk *chunk, int offset) {
  uint8_t constantIndex = chunk->code[offset + 1];
  printf("%-16s %4d '", name, constantIndex);
  printValue(chunk->constants.values[constantIndex]);
  printf("'\n");
  return offset + 2;
}

// Disassembles the instruction at the `offset` index of the chunk's array
// of instructions. Returns the index of the next instruction to disassemble.
int disassembleInstruction(Chunk *chunk, int offset) {
  printf("%04d ", offset);

  if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
    printf("   | ");
  } else {
    printf("%4d ", chunk->lines[offset]);
  }

  uint8_t instruction = chunk->code[offset];
  switch (instruction) {
  case OP_CONSTANT:
    return constantInstruction("OP_CONSTANT", chunk, offset);
  case OP_POP:
    return simpleInstruction("OP_POP", offset);
  case OP_PUSH:
    return simpleInstruction("OP_PUSH", offset);
  case OP_ADD:
    return simpleInstruction("OP_ADD", offset);
  case OP_SUBTRACT:
    return simpleInstruction("OP_SUBTRACT", offset);
  case OP_MULTIPLY:
    return simpleInstruction("OP_MULTIPLY", offset);
  case OP_DIVIDE:
    return simpleInstruction("OP_DIVIDE", offset);
  case OP_NEGATE:
    return simpleInstruction("OP_NEGATE", offset);
  case OP_RETURN:
    return simpleInstruction("OP_RETURN", offset);
  default:
    printf("Unknown opcode %d\n", instruction);
    return offset + 1;
  }
}

// Disassembles the entire chunk to make instructions human-readable.
void disassembleChunk(Chunk *chunk, const char *name) {
  printf("== %s ==\n", name);

  int offset = 0;
  while (offset < chunk->count) {
    offset = disassembleInstruction(chunk, offset);
  }
}
