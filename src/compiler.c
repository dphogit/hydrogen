#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "object.h"
#include "scanner.h"
#include "token.h"

// ----- Parser Utilities -----

static void errorAt(Parser *parser, Token *token, const char *message) {
  if (parser->panicMode)
    return; // Returning early in panic mode suppresses cascaded errors.

  parser->panicMode = true;
  fprintf(stderr, "[line %d] Error", token->line);

  if (token->type == TOKEN_EOF) {
    fprintf(stderr, " at end");
  } else if (token->type == TOKEN_ERROR) {
  } else {
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }

  fprintf(stderr, ": %s\n", message);
  parser->hadError = true;
}

static void errorAtCurrent(Parser *parser, const char *message) {
  errorAt(parser, &parser->current, message);
}

static void error(Parser *parser, const char *message) {
  errorAt(parser, &parser->previous, message);
}

static void advance(Parser *parser) {
  parser->previous = parser->current;

  while (true) {
    parser->current = scanToken(&parser->scanner);
    if (parser->current.type != TOKEN_ERROR)
      break;

    // Error tokens include their messages in the struct.
    errorAtCurrent(parser, parser->current.start);
  }
}

static void consume(Parser *parser, TokenType type, const char *message) {
  if (parser->current.type == type) {
    advance(parser);
    return;
  }
  errorAtCurrent(parser, message);
}

static bool check(Parser *parser, TokenType type) {
  return parser->current.type == type;
}

static bool match(Parser *parser, TokenType type) {
  if (!check(parser, type))
    return false;

  advance(parser);
  return true;
}

static void emitByte(Parser *parser, uint8_t byte) {
  writeChunk(parser->chunk, byte, parser->previous.line);
}

static void emitBytes(Parser *parser, uint8_t byte1, uint8_t byte2) {
  writeChunk(parser->chunk, byte1, parser->previous.line);
  writeChunk(parser->chunk, byte2, parser->previous.line);
}

static void emitConstant(Parser *parser, Value value) {
  int constantIndex = addConstant(parser->chunk, value);
  if (constantIndex > UINT8_MAX) {
    // Each instruction in the chunk's can only be 1 byte thus the chunk
    // can only have 256 constants due to this max size of the index.
    error(parser, "Too many constants in one chunk.");
    constantIndex = 0;
  }
  emitBytes(parser, OP_CONSTANT, (uint8_t)constantIndex);
}

static void endCompiler(Parser *parser) {
  emitByte(parser, OP_RETURN);
#ifdef DEBUG_PRINT_CODE
  if (!parser->hadError) {
    disassembleChunk(parser->chunk, "code");
  }
#endif
}

// ----- Precedence Rule Parsing -----

static ParseRule *getRule(TokenType type);
static void expression(Parser *parser);
static void statement(Parser *parser);
static void declaration(Parser *parser);
static void parsePrecedence(Parser *parser, Precedence precedence);

static void number(Parser *parser) {
  double value = strtod(parser->previous.start, NULL);
  emitConstant(parser, NUMBER_VAL(value));
}

static void string(Parser *parser) {
  // +1 and -2 will trim the leading and trailing quotation marks.
  Value str = OBJ_VAL(copyString(parser->gc, parser->strings,
                                 parser->previous.start + 1,
                                 parser->previous.length - 2));
  emitConstant(parser, str);
}

static void grouping(Parser *parser) {
  expression(parser);
  consume(parser, TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void unary(Parser *parser) {
  TokenType operatorType = parser->previous.type;
  parsePrecedence(parser, PREC_UNARY); // Parse (single) right operand.

  switch (operatorType) {
  case TOKEN_MINUS:
    emitByte(parser, OP_NEGATE);
    break;
  case TOKEN_BANG:
    emitByte(parser, OP_NOT);
  default:
    return; // Unreachable.
  }
}

static void binary(Parser *parser) {
  TokenType operatorType = parser->previous.type;
  ParseRule *rule = getRule(operatorType);

  // Parse right operand with one higher level of precedence because binary
  // operators are left associative. e.g. 1 + 2 + 3 => (1 + 2) + 3.
  parsePrecedence(parser, (Precedence)(rule->precedence + 1));

  switch (operatorType) {
  case TOKEN_PLUS:
    emitByte(parser, OP_ADD);
    break;
  case TOKEN_MINUS:
    emitByte(parser, OP_SUBTRACT);
    break;
  case TOKEN_STAR:
    emitByte(parser, OP_MULTIPLY);
    break;
  case TOKEN_SLASH:
    emitByte(parser, OP_DIVIDE);
    break;
  case TOKEN_LESS:
    emitByte(parser, OP_LT);
    break;
  case TOKEN_LESS_EQUAL:
    emitByte(parser, OP_LTE);
    break;
  case TOKEN_BANG_EQUAL:
    emitByte(parser, OP_NEQ);
    break;
  case TOKEN_EQUAL_EQUAL:
    emitByte(parser, OP_EQ);
    break;
  case TOKEN_GREATER:
    emitByte(parser, OP_GT);
    break;
  case TOKEN_GREATER_EQUAL:
    emitByte(parser, OP_GTE);
    break;
  default:
    return; // Unreachable.
  }
}

static void literal(Parser *parser) {
  switch (parser->previous.type) {
  case TOKEN_FALSE:
    emitByte(parser, OP_FALSE);
    return;
  case TOKEN_TRUE:
    emitByte(parser, OP_TRUE);
    return;
  case TOKEN_NIL:
    emitByte(parser, OP_NIL);
    return;
  default:
    return; // Unreachable.
  }
}

/*
 * An array acting as a 'table', indexed by the token type. Each element
 * is a parse rule structure consisting of:
 *
 * 1. The function to compile a prefix expression starting with that token type.
 *
 * 2. The function to compile an infix expression whose left operand is
 *    followed by a token of that type.
 *
 * 3. The precedence of an infix expression that uses that token as an operator.
 */
ParseRule rules[] = {
    // Single character
    [TOKEN_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
    [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT] = {NULL, NULL, PREC_NONE},
    [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TOKEN_MINUS] = {unary, binary, PREC_TERM},
    [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
    [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},

    // One or two characters
    [TOKEN_BANG] = {unary, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_GREATER] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL] = {NULL, binary, PREC_COMPARISON},

    // Keywords
    [TOKEN_AND] = {NULL, NULL, PREC_NONE},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {literal, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, NULL, PREC_NONE},
    [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
    [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
    [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
    [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
    [TOKEN_TRUE] = {literal, NULL, PREC_NONE},
    [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
    [TOKEN_WHILE] = {NULL, NULL, PREC_NONE},

    // Literals
    [TOKEN_STRING] = {string, NULL, PREC_NONE},
    [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
    [TOKEN_IDENTIFIER] = {NULL, NULL, PREC_NONE},

    [TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
};

static ParseRule *getRule(TokenType type) { return &rules[type]; }

static void expression(Parser *parser) {
  parsePrecedence(parser, PREC_ASSIGNMENT);
}

// Exit panic mode when reaching a synchronization point. These are identified
// as statement boundaries, which include semi-colons ending a statement, or a
// token that begins a statement.
static void synchronize(Parser *parser) {
  parser->panicMode = false;

  while (parser->current.type != TOKEN_EOF) {
    if (parser->previous.type == TOKEN_SEMICOLON)
      return;

    switch (parser->current.type) {
    case TOKEN_CLASS:
    case TOKEN_FUN:
    case TOKEN_VAR:
    case TOKEN_FOR:
    case TOKEN_IF:
    case TOKEN_WHILE:
    case TOKEN_PRINT:
    case TOKEN_RETURN:
      return;
    default:; // Do nothing.
    }

    advance(parser);
  }
}

static void declaration(Parser *parser) {
  statement(parser);

  if (parser->panicMode)
    synchronize(parser);
}

static void printStatement(Parser *parser) {
  expression(parser);
  consume(parser, TOKEN_SEMICOLON, "Expect ';' after value.");
  emitByte(parser, OP_PRINT);
}

static void expressionStatement(Parser *parser) {
  expression(parser);
  consume(parser, TOKEN_SEMICOLON, "Expect ';' after value.");
  emitByte(parser, OP_POP);
}

static void statement(Parser *parser) {
  if (match(parser, TOKEN_PRINT))
    printStatement(parser);
  else
    expressionStatement(parser);
}

static void parsePrecedence(Parser *parser, Precedence precedence) {
  advance(parser);
  ParseFn prefixRule = getRule(parser->previous.type)->prefix;
  if (prefixRule == NULL) {
    error(parser, "Expect expression.");
    return;
  }

  prefixRule(parser);

  while (precedence <= getRule(parser->current.type)->precedence) {
    advance(parser);
    ParseFn infixRule = getRule(parser->previous.type)->infix;
    infixRule(parser);
  }
}

// Returns true if the compilation succeeded.
bool compile(const char *source, Chunk *chunk, GC *gc, Table *strings) {
  Scanner scanner;
  initScanner(&scanner, source);

  Parser parser;
  parser.scanner = scanner;
  parser.hadError = false;
  parser.panicMode = false;
  parser.chunk = chunk;
  parser.gc = gc;
  parser.strings = strings;

  advance(&parser);

  while (!match(&parser, TOKEN_EOF)) {
    declaration(&parser);
  }

  endCompiler(&parser);

  return !parser.hadError;
}
