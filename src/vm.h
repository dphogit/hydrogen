#ifndef HYDRO_VM_H
#define HYDRO_VM_H

typedef enum InterpretResult {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

InterpretResult interpret(const char *source);

#endif
