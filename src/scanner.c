/*
 * The process of scanning (lexing) converts the source string into tokens
 * which the language can understand for parsing.
 */

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include "scanner.h"
#include "token.h"

void initScanner(Scanner *scanner, const char *source) {
  scanner->start = source;
  scanner->current = source;
  scanner->line = 1;
}

static bool atEnd(Scanner *scanner) { return *scanner->current == '\0'; }

static char peek(Scanner *scanner) { return *scanner->current; }

static char peekNext(Scanner *scanner) {
  return atEnd(scanner) ? '\0' : scanner->current[1];
}

static char advance(Scanner *scanner) {
  scanner->current++;
  return scanner->current[-1];
}

static char match(Scanner *scanner, char expected) {
  if (atEnd(scanner) || *scanner->current != expected)
    return false;

  scanner->current++;
  return true;
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

static Token checkKeyword(Scanner *scanner, int start, int restLength,
                          const char *rest, TokenType type) {
  // Pass the length as we know it beforehand, faster than calculating it.
  bool sameLength = scanner->current - scanner->start == start + restLength;
  bool match = memcmp(scanner->start + start, rest, restLength) == 0;
  TokenType resultingType = sameLength && match ? type : TOKEN_IDENTIFIER;
  return createToken(scanner, resultingType);
}

static Token createIdentifierOrKeywordToken(Scanner *scanner) {
  while (isalpha(peek(scanner)) || isdigit(peek(scanner)))
    advance(scanner);

  // Check for keyword via a manual trie-like data structure.
  switch (scanner->start[0]) {
  case 'a':
    return checkKeyword(scanner, 1, 2, "nd", TOKEN_AND);
  case 'c':
    return checkKeyword(scanner, 1, 4, "lass", TOKEN_CLASS);
  case 'e':
    return checkKeyword(scanner, 1, 3, "lse", TOKEN_ELSE);
  case 'f':
    if (scanner->current - scanner->start > 1) {
      switch (scanner->start[1]) {
      case 'a':
        return checkKeyword(scanner, 2, 3, "lse", TOKEN_FALSE);
      case 'o':
        return checkKeyword(scanner, 2, 1, "r", TOKEN_FOR);
      case 'u':
        return checkKeyword(scanner, 2, 1, "n", TOKEN_FUN);
      }
    }
    break;
  case 'i':
    return checkKeyword(scanner, 1, 1, "f", TOKEN_IF);
  case 'n':
    return checkKeyword(scanner, 1, 2, "il", TOKEN_NIL);
  case 'o':
    return checkKeyword(scanner, 1, 1, "r", TOKEN_OR);
  case 'p':
    return checkKeyword(scanner, 1, 4, "rint", TOKEN_PRINT);
  case 'r':
    return checkKeyword(scanner, 1, 5, "eturn", TOKEN_RETURN);
  case 's':
    return checkKeyword(scanner, 1, 4, "uper", TOKEN_SUPER);
  case 't':
    if (scanner->current - scanner->start > 1) {
      switch (scanner->start[1]) {
      case 'h':
        return checkKeyword(scanner, 2, 2, "is", TOKEN_THIS);
      case 'r':
        return checkKeyword(scanner, 2, 2, "ue", TOKEN_TRUE);
      }
    }
    break;
  case 'v':
    return checkKeyword(scanner, 1, 2, "ar", TOKEN_VAR);
  case 'w':
    return checkKeyword(scanner, 1, 4, "hile", TOKEN_WHILE);
  }

  // Unsuccessful checks for keyword must indicate identifiers.
  return createToken(scanner, TOKEN_IDENTIFIER);
}

static Token createNumberToken(Scanner *scanner) {
  while (isdigit(peek(scanner)))
    advance(scanner);

  // Handle fractional part of number.
  if (peek(scanner) == '.' && isdigit(peekNext(scanner))) {
    do {
      advance(scanner);
    } while (isdigit(peek(scanner)));
  }

  return createToken(scanner, TOKEN_NUMBER);
}

static Token createStringToken(Scanner *scanner) {
  while (peek(scanner) != '"' && !atEnd(scanner)) {
    if (peek(scanner) == '\n')
      scanner->line++; // Multi-line strings are allowed in hydro.

    advance(scanner);
  }

  if (atEnd(scanner))
    return createErrorToken(scanner, "Unterminated string.");

  advance(scanner); // Consume the closing quote.
  return createToken(scanner, TOKEN_STRING);
}

Token scanToken(Scanner *scanner) {
  skipWhitespace(scanner);
  scanner->start = scanner->current;

  if (atEnd(scanner))
    return createToken(scanner, TOKEN_EOF);

  char c = advance(scanner);

  if (isalpha(c))
    return createIdentifierOrKeywordToken(scanner);
  if (isdigit(c))
    return createNumberToken(scanner);

  switch (c) {
  case '(':
    return createToken(scanner, TOKEN_LEFT_PAREN);
  case ')':
    return createToken(scanner, TOKEN_RIGHT_PAREN);
  case '{':
    return createToken(scanner, TOKEN_LEFT_BRACE);
  case '}':
    return createToken(scanner, TOKEN_RIGHT_BRACE);
  case ',':
    return createToken(scanner, TOKEN_COMMA);
  case '.':
    return createToken(scanner, TOKEN_DOT);
  case ';':
    return createToken(scanner, TOKEN_SEMICOLON);
  case '+':
    return createToken(scanner, TOKEN_PLUS);
  case '-':
    return createToken(scanner, TOKEN_MINUS);
  case '*':
    return createToken(scanner, TOKEN_STAR);
  case '/':
    return createToken(scanner, TOKEN_SLASH);
  case '"':
    return createStringToken(scanner);
  case '!':
    return createToken(scanner,
                       match(scanner, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
  case '=':
    return createToken(scanner,
                       match(scanner, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
  case '>':
    return createToken(scanner, match(scanner, '=') ? TOKEN_GREATER_EQUAL
                                                    : TOKEN_GREATER);
  case '<':
    return createToken(scanner,
                       match(scanner, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
  default:
    return createErrorToken(scanner, "Unexpected character.");
  }
}
