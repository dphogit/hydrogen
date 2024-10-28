#ifndef HYDRO_VM_H
#define HYDRO_VM_H

#include <stdint.h>

#include "chunk.h"
#include "gc.h"
#include "table.h"
#include "value.h"

#define STACK_MAX 256

/*
 * The almighty stack-based Virtual Machine that executes the instructions.
 * It has a fixed stack size of STACK_MAX (256).
 */
typedef struct VM {
  uint8_t *ip;  // Pointer to the next instruction to be executed.
  Chunk *chunk; // Containing the instructions to execute.
  Value stack[STACK_MAX];
  Value *stackTop; // Points to the element one after the stacks top value.
  GC gc;           // Auto-reclaim memory during program execution.
  Table strings;   // The string interning pool.
  Table globals;   // Global variables.
} VM;

void initVM(VM *vm);
void freeVM(VM *vm);

void push(VM *vm, Value value);
Value pop(VM *vm);

typedef enum InterpretResult {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

InterpretResult interpret(VM *vm, const char *source);

#endif
