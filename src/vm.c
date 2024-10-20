#include "vm.h"
#include "chunk.h"
#include "compiler.h"

InterpretResult interpret(const char *source) {
  Chunk chunk;
  initChunk(&chunk);

  if (!compile(source, &chunk)) {
    freeChunk(&chunk);
    return INTERPRET_COMPILE_ERROR;
  }

  // TODO: Execute the bytecode!

  return INTERPRET_OK;
}
