#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "object.h"
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

static void runtimeError(VM *vm, const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  size_t instructionLineIndex = vm->ip - vm->chunk->code - 1;
  int line = vm->chunk->lines[instructionLineIndex];
  fprintf(stderr, "[line %d] in script\n", line);
  resetStack(vm);
}

static Value peekn(VM *vm, int n) { return vm->stackTop[-(n + 1)]; }

static Value peek(VM *vm) { return vm->stackTop[-1]; }

// Nil and false are falsy, everything else is truthy.
static bool isFalsy(Value value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate(VM *vm) {
  ObjString *b = AS_STRING(pop(vm)), *a = AS_STRING(pop(vm));

  int n = a->length + b->length;
  char *buffer = malloc(sizeof(char) * (n + 1));
  memcpy(buffer, a->chars, a->length);
  memcpy(buffer + a->length, b->chars, b->length);
  buffer[n] = '\0';

  push(vm, OBJ_VAL(takeString(buffer, n)));
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

static bool binaryOperandsAreNumbers(VM *vm) {
  if (IS_NUMBER(peek(vm)) && IS_NUMBER(peekn(vm, 1)))
    return true;

  runtimeError(vm, "Operands must be numbers.");
  return false;
}

static InterpretResult run(VM *vm) {
#define BINARY_OP(valueType, op)                                               \
  do {                                                                         \
    if (!binaryOperandsAreNumbers(vm))                                         \
      return INTERPRET_RUNTIME_ERROR;                                          \
                                                                               \
    double b = AS_NUMBER(pop(vm)), a = AS_NUMBER(pop(vm));                     \
    push(vm, valueType(a op b));                                               \
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
    case OP_NIL:
      push(vm, NIL_VAL);
      break;
    case OP_TRUE:
      push(vm, BOOL_VAL(true));
      break;
    case OP_FALSE:
      push(vm, BOOL_VAL(false));
      break;
    case OP_ADD:
      if (IS_STRING(peek(vm)) && IS_STRING(peekn(vm, 1))) {
        concatenate(vm);
        break;
      }
      if (IS_NUMBER(peek(vm)) && IS_NUMBER(peekn(vm, 1))) {
        double b = AS_NUMBER(pop(vm)), a = AS_NUMBER(pop(vm));
        push(vm, NUMBER_VAL(a + b));
        break;
      }
      runtimeError(vm, "Operands must be two numbers or two strings");
      return INTERPRET_RUNTIME_ERROR;
    case OP_SUBTRACT:
      BINARY_OP(NUMBER_VAL, -);
      break;
    case OP_MULTIPLY:
      BINARY_OP(NUMBER_VAL, *);
      break;
    case OP_DIVIDE:
      BINARY_OP(NUMBER_VAL, /);
      break;
    case OP_LT:
      BINARY_OP(BOOL_VAL, <);
      break;
    case OP_LTE:
      if (!binaryOperandsAreNumbers(vm))
        return INTERPRET_RUNTIME_ERROR;
      Value b = pop(vm), a = pop(vm);
      push(vm, BOOL_VAL(AS_NUMBER(a) < AS_NUMBER(b) || valuesEqual(a, b)));
      break;
    case OP_EQ: {
      Value b = pop(vm), a = pop(vm);
      push(vm, BOOL_VAL(valuesEqual(a, b)));
      break;
    }
    case OP_NEQ: {
      Value b = pop(vm), a = pop(vm);
      push(vm, BOOL_VAL(!valuesEqual(a, b)));
      break;
    }
    case OP_GT:
      BINARY_OP(BOOL_VAL, >);
      break;
    case OP_GTE: {
      if (!binaryOperandsAreNumbers(vm))
        return INTERPRET_RUNTIME_ERROR;
      Value b = pop(vm), a = pop(vm);
      push(vm, BOOL_VAL(AS_NUMBER(a) > AS_NUMBER(b) || valuesEqual(a, b)));
      break;
    }
    case OP_NOT: // Modify the stack's top in place.
      *(vm->stackTop - 1) = BOOL_VAL(isFalsy(*(vm->stackTop - 1)));
      break;
    case OP_NEGATE: // Modify the stack's top in place.
      if (IS_NUMBER(peek(vm))) {
        *(vm->stackTop - 1) = NUMBER_VAL(-AS_NUMBER(*(vm->stackTop - 1)));
        break;
      }
      runtimeError(vm, "Operand to negation must be a number.");
      return INTERPRET_RUNTIME_ERROR;
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
