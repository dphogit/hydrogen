#include <stdint.h>
#include <stdio.h>

#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "value.h"
#include "vm.h"

void resetStack(VM *vm) { vm->stackTop = vm->stack; }

void initVM(VM *vm) { resetStack(vm); }

void freeVM(VM *vm) { initVM(vm); }

void push(VM *vm, Value value) {
  *vm->stackTop = value;
  vm->stackTop++;
}

Value pop(VM *vm) {
  vm->stackTop--;
  return *vm->stackTop;
}

#ifdef DEBUG_TRACE_EXECUTION
static void printStack(VM *vm) {
  printf("          ");
  if (vm->stack == vm->stackTop) {
    printf("[EMPTY STACK]");
  } else {
    for (Value *slot = vm->stack; slot < vm->stackTop; slot++) {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
  }
  printf("\n");
}
#endif

static inline uint8_t readByte(VM *vm) { return *vm->ip++; }

static inline Value readConstant(VM *vm) {
  return vm->chunk->constants.values[readByte(vm)];
}

static InterpretResult run(VM *vm) {
#define BINARY_OP(op)                                                          \
  do {                                                                         \
    Value b = pop(vm), a = pop(vm);                                            \
    push(vm, a op b);                                                          \
  } while (false)

  while (true) {
#ifdef DEBUG_TRACE_EXECUTION
    printStack(vm);
    disassembleInstruction(vm->chunk, vm->ip - vm->chunk->code);
#endif

    uint8_t instruction;
    switch (instruction = readByte(vm)) {
    case OP_CONSTANT: {
      Value constant = readConstant(vm);
      push(vm, constant);
      break;
    }
    case OP_ADD:
      BINARY_OP(+);
      break;
    case OP_SUBTRACT:
      BINARY_OP(-);
      break;
    case OP_MULTIPLY:
      BINARY_OP(*);
      break;
    case OP_DIVIDE:
      BINARY_OP(/);
      break;
    case OP_NEGATE:
      *(vm->stackTop - 1) = -*(vm->stackTop - 1); // Modify stack top in place.
      break;
    case OP_RETURN: {
      printValue(pop(vm));
      printf("\n");
      return INTERPRET_OK;
    }
    }
  }

#undef BINARY_OP
}

InterpretResult interpret(const char *source) {
  Chunk chunk;
  initChunk(&chunk);

  if (!compile(source, &chunk)) {
    freeChunk(&chunk);
    return INTERPRET_COMPILE_ERROR;
  }

  VM vm;
  initVM(&vm);
  vm.chunk = &chunk;
  vm.ip = chunk.code;

  InterpretResult result = run(&vm);

  freeChunk(vm.chunk);
  freeVM(&vm);

  return result;
}
