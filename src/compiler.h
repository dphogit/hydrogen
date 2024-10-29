#ifndef HYDRO_COMPILER_H
#define HYDRO_COMPILER_H

#include <stdbool.h>

#include "chunk.h"
#include "gc.h"
#include "scanner.h"
#include "table.h"
#include "token.h"

// Instruction operand to encode a local is a single byte, so the VM has a hard
// limit on the number of locals that can be in scope at once.
#define UINT8_COUNT (UINT8_MAX + 1)

#define GLOBAL_SCOPE_DEPTH 0

typedef struct Local {
  Token name;
  int depth; // The scope depth of where the local var was declared.
} Local;

typedef struct Compiler {
  Local locals[UINT8_COUNT]; // Locals in current scope, code declaration order.
  int localCount;            // Number of locals are in scope.
  int scopeDepth;            // Number of blocks surrounding current code.
} Compiler;

typedef struct Parser {
  Token current;
  Token previous;
  Scanner scanner;
  Compiler compiler;
  bool hadError;
  bool panicMode;
  Chunk *chunk;   // The chunk the bytecode is being written to.
  GC *gc;         // Will add heap-allocated objects to the GC during parsing.
  Table *strings; // String interning pool.
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

typedef void (*ParseFn)(Parser *parser, bool canAssign);

typedef struct {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
} ParseRule;

bool compile(const char *source, Chunk *chunk, GC *gc, Table *strings);

#endif
