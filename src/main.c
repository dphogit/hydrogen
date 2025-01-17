#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm.h"

// https://man.freebsd.org/cgi/man.cgi?query=sysexits&manpath=FreeBSD+4.3-RELEASE
#define EX_USAGE 64
#define EX_DATAERR 65
#define EX_SOFTWARE 70
#define EX_IOERR 74

void runREPL(VM *vm) {
  size_t n = 0;
  char *line = NULL;

  printf("Welcome to Hydrogen, the lightest programming language!\n");
  while (true) {
    printf("> ");
    if (getline(&line, &n, stdin) != -1) {
      interpret(vm, line);
    }
  }
}

const char *readFile(const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    fprintf(stderr, "Could not open file, %s.\n", filename);
    exit(EX_IOERR);
  }

  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  char *buffer = malloc(fileSize + 1); // +1 for '/0'
  if (buffer == NULL) {
    fprintf(stderr, "Could not read file, %s.\n", filename);
    exit(EX_IOERR);
  }

  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  if (bytesRead < fileSize) {
    fprintf(stderr, "Could not read file, %s.\n", filename);
    exit(EX_IOERR);
  }

  buffer[bytesRead] = '\0';
  fclose(file);
  return buffer;
}

void runFile(VM *vm, const char *filename) {
  char *dot = strrchr(filename, '.');
  if (dot == NULL || (strcmp("hydro", dot + 1) != 0)) {
    fprintf(stderr, "File must have .hydro extension.\n");
    exit(EX_DATAERR);
  }

  const char *source = readFile(filename);
  InterpretResult result = interpret(vm, source);
  free((void *)source);

  if (result == INTERPRET_COMPILE_ERROR)
    exit(EX_DATAERR);
  else if (result == INTERPRET_RUNTIME_ERROR)
    exit(EX_SOFTWARE);
}

void usage() {
  printf("\nUSAGE:\n");
  printf("\thydro [FILE]\n");
  printf("DESCRIPTION\n");
  printf("\tRuns hydrogen FILE which must have .hydro extension. If FILE is "
         "not provided, runs REPL.\n\n");
}

int main(int argc, char *argv[]) {
  VM vm;
  initVM(&vm);

  if (argc == 1) {
    runREPL(&vm);
  } else if (argc == 2) {
    runFile(&vm, argv[1]);
  } else {
    usage();
    return EX_USAGE;
  }

  freeVM(&vm);
  return EXIT_SUCCESS;
}
