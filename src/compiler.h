#ifndef HYDRO_COMPILER_H
#define HYDRO_COMPILER_H

#include <stdbool.h>

#include "chunk.h"
#include "scanner.h"
#include "token.h"

typedef struct Parser {
  Token current;
  Token previous;
  Scanner scanner;
  bool hadError;
  bool panicMode;
  Chunk *chunk; // The chunk the bytecode is being written to.
} Parser;

/* Hydrogen's precedence levels, in order from lowest to highest. */
typedef enum Precedence {
  PREC_NONE,
  PREC_TERM,   // + -
  PREC_FACTOR, // * /
  PREC_UNARY,  // ! -
  PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)(Parser *parser);

typedef struct {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
} ParseRule;

bool compile(const char *source, Chunk *chunk);

#endif
