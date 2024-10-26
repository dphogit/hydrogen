#ifndef HYDRO_COMPILER_H
#define HYDRO_COMPILER_H

#include <stdbool.h>

#include "chunk.h"
#include "gc.h"
#include "scanner.h"
#include "token.h"

typedef struct Parser {
  Token current;
  Token previous;
  Scanner scanner;
  bool hadError;
  bool panicMode;
  Chunk *chunk; // The chunk the bytecode is being written to.
  GC *gc;       // Will add heap-allocated objects to the GC during parsing.
} Parser;

// Hydrogen's precedence levels, in order from lowest to highest.
// Follows: https://en.cppreference.com/w/c/language/operator_precedence
typedef enum Precedence {
  PREC_NONE,
  PREC_ASSIGNMENT, // =
  PREC_OR,         // or
  PREC_AND,        // and
  PREC_EQUALITY,   // == !=
  PREC_COMPARISON, // < > <= >=
  PREC_TERM,       // + -
  PREC_FACTOR,     // * /
  PREC_UNARY,      // ! -
  PREC_CALL,       // . ()
  PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)(Parser *parser);

typedef struct {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
} ParseRule;

bool compile(const char *source, Chunk *chunk, GC *gc);

#endif
