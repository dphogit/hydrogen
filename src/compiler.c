#include <stdbool.h>
#include <stdio.h>

#include "chunk.h"
#include "compiler.h"
#include "scanner.h"
#include "token.h"

// Returns true if the compilation succeeded.
bool compile(const char *source, Chunk *chunk) {
  Scanner scanner;
  initScanner(&scanner, source);

  int line = -1;
  while (true) {
    Token token = scanToken(&scanner);

    // TODO - Disassemle
    if (token.type == TOKEN_EOF)
      break;

    if (token.line == line) {
      printf("   | ");
    } else {
      printf("%4d ", token.line);
      line = token.line;
    }

    printf("%2d '%.*s'\n", token.type, token.length, token.start);
  }

  return true;
}
