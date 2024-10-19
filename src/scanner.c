/*
 * The process of scanning (lexing) converts the source string into tokens
 * which the language can understand for parsing.
 */

#include <stdbool.h>
#include <string.h>

#include "scanner.h"
#include "token.h"

void initScanner(Scanner *scanner, const char *source) {
  scanner->start = source;
  scanner->current = source;
  scanner->line = 1;
}

static bool isDigit(char c) { return c >= '0' && c <= '9'; }

static bool atEnd(Scanner *scanner) { return *scanner->current == '\0'; }

static char peek(Scanner *scanner) { return *scanner->current; }

static char peekNext(Scanner *scanner) {
  return atEnd(scanner) ? '\0' : scanner->current[1];
}

static char advance(Scanner *scanner) {
  scanner->current++;
  return scanner->current[-1];
}

static void skipWhitespace(Scanner *scanner) {
  while (true) {
    switch (peek(scanner)) {
    case ' ':
    case '\r':
    case '\t':
      advance(scanner);
      break;
    case '\n':
      scanner->line++;
      advance(scanner);
      break;
    case '/':
      if (peekNext(scanner) == '/') {
        // Inline comments contents are for the remainder of the line.
        while (peek(scanner) != '\n' && !atEnd(scanner))
          advance(scanner);
        break;
      }
      return;
    default:
      return;
    }
  }
}

static Token createToken(Scanner *scanner, TokenType type) {
  Token token;
  token.type = type;
  token.start = scanner->start;
  token.length = scanner->current - scanner->start;
  token.line = scanner->line;
  return token;
}

static Token createErrorToken(Scanner *scanner, char *message) {
  Token token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.length = strlen(message);
  token.line = scanner->line;
  return token;
}

static Token createNumberToken(Scanner *scanner) {
  while (isDigit(peek(scanner)))
    advance(scanner);

  // Handle fractional part of number.
  if (peek(scanner) == '.' && isDigit(peekNext(scanner))) {
    do {
      advance(scanner);
    } while (isDigit(peek(scanner)));
  }

  return createToken(scanner, TOKEN_NUMBER);
}

Token scanToken(Scanner *scanner) {
  skipWhitespace(scanner);
  scanner->start = scanner->current;

  if (atEnd(scanner))
    return createToken(scanner, TOKEN_EOF);

  char c = advance(scanner);

  if (isDigit(c))
    return createNumberToken(scanner);

  switch (c) {
  case '(':
    return createToken(scanner, TOKEN_LEFT_PAREN);
  case ')':
    return createToken(scanner, TOKEN_RIGHT_PAREN);
  case '+':
    return createToken(scanner, TOKEN_PLUS);
  case '-':
    return createToken(scanner, TOKEN_MINUS);
  case '*':
    return createToken(scanner, TOKEN_STAR);
  case '/':
    return createToken(scanner, TOKEN_SLASH);
  default:
    return createErrorToken(scanner, "Unexpected character.\n");
  }
}
