#include "scanner.h"
#include "token.h"
#include "utest.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

UTEST(scanner, init) {
  Scanner scanner;

  initScanner(&scanner, "source");

  ASSERT_EQ(scanner.line, 1);
  ASSERT_EQ(*scanner.start, 's');
  ASSERT_EQ(*scanner.current, 's');
}

typedef struct TokenTestCase {
  char *source;
  TokenType type;
  int len;
} TokenTestCase;

TokenTestCase tokenTestCases[] = {
    // Single character tokens.
    {.source = "(", .type = TOKEN_LEFT_PAREN, .len = 1},
    {.source = ")", .type = TOKEN_RIGHT_PAREN, .len = 1},
    {.source = "{", .type = TOKEN_LEFT_BRACE, .len = 1},
    {.source = "}", .type = TOKEN_RIGHT_BRACE, .len = 1},
    {.source = ",", .type = TOKEN_COMMA, .len = 1},
    {.source = ".", .type = TOKEN_DOT, .len = 1},
    {.source = "+", .type = TOKEN_PLUS, .len = 1},
    {.source = "-", .type = TOKEN_MINUS, .len = 1},
    {.source = "*", .type = TOKEN_STAR, .len = 1},
    {.source = "/", .type = TOKEN_SLASH, .len = 1},
    {.source = ";", .type = TOKEN_SEMICOLON, .len = 1},

    // One or two character tokens.
    {.source = "!", .type = TOKEN_BANG, .len = 1},
    {.source = "!=", .type = TOKEN_BANG_EQUAL, .len = 2},
    {.source = "=", .type = TOKEN_EQUAL, .len = 1},
    {.source = "==", .type = TOKEN_EQUAL_EQUAL, .len = 2},
    {.source = ">", .type = TOKEN_GREATER, .len = 1},
    {.source = ">=", .type = TOKEN_GREATER_EQUAL, .len = 2},
    {.source = "<", .type = TOKEN_LESS, .len = 1},
    {.source = "<=", .type = TOKEN_LESS_EQUAL, .len = 2},

    // Keywords
    {.source = "and", .type = TOKEN_AND, .len = 3},
    {.source = "class", .type = TOKEN_CLASS, .len = 5},
    {.source = "else", .type = TOKEN_ELSE, .len = 4},
    {.source = "false", .type = TOKEN_FALSE, .len = 5},
    {.source = "for", .type = TOKEN_FOR, .len = 3},
    {.source = "fun", .type = TOKEN_FUN, .len = 3},
    {.source = "if", .type = TOKEN_IF, .len = 2},
    {.source = "nil", .type = TOKEN_NIL, .len = 3},
    {.source = "or", .type = TOKEN_OR, .len = 2},
    {.source = "print", .type = TOKEN_PRINT, .len = 5},
    {.source = "return", .type = TOKEN_RETURN, .len = 6},
    {.source = "super", .type = TOKEN_SUPER, .len = 5},
    {.source = "this", .type = TOKEN_THIS, .len = 4},
    {.source = "true", .type = TOKEN_TRUE, .len = 4},
    {.source = "var", .type = TOKEN_VAR, .len = 3},
    {.source = "while", .type = TOKEN_WHILE, .len = 5},

    // Literals
    {.source = "\"myString\"", .type = TOKEN_STRING, .len = 10},
    {.source = "123.45", .type = TOKEN_NUMBER, .len = 6},
    {.source = "myVar", .type = TOKEN_IDENTIFIER, .len = 5},
};

UTEST(Scanner, scanToken) {
  int n = sizeof(tokenTestCases) / sizeof(TokenTestCase);

  // printf("Executing %d cases...\n", n);
  for (int i = 0; i < n; i++) {
    TokenTestCase testCase = tokenTestCases[i];

    // printf("[%2d] %10s", i, testCase.source);
    Scanner scanner;
    initScanner(&scanner, testCase.source);

    Token token = scanToken(&scanner);

    ASSERT_EQ(token.length, testCase.len);
    ASSERT_EQ(token.line, 1);
    ASSERT_STREQ(token.start, testCase.source);
    ASSERT_EQ(token.type, testCase.type);
    // printf("    PASSED\n");
  }
}

typedef struct WhitespaceTestCase {
  char *source;
  int lines;
} WhitespaceTestCase;

WhitespaceTestCase whitespaceTestCases[] = {
    {.source = "  \n", .lines = 2},
    {.source = "\r\n", .lines = 2},
    {.source = "\n\n", .lines = 3},
    {.source = "\t\n", .lines = 2},
    {.source = "// Inline comment\n", .lines = 2}};

UTEST(Scanner, whiteSpace) {
  int n = sizeof(whitespaceTestCases) / sizeof(WhitespaceTestCase);

  for (int i = 0; i < n; i++) {
    WhitespaceTestCase testCase = whitespaceTestCases[i];

    Scanner scanner;
    initScanner(&scanner, testCase.source);

    scanToken(&scanner);

    ASSERT_EQ(scanner.line, testCase.lines);
  }
}

UTEST(Scanner, syntaxErrors) {
  Scanner scanner;

  initScanner(&scanner, "$");
  Token token = scanToken(&scanner);
  ASSERT_EQ(token.type, (TokenType)TOKEN_ERROR);
  ASSERT_STREQ(token.start, "Unexpected character.");

  initScanner(&scanner, "\"I forgot a closing quote!");
  token = scanToken(&scanner);
  ASSERT_EQ(token.type, (TokenType)TOKEN_ERROR);
  ASSERT_STREQ(token.start, "Unterminated string.");
}
