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

struct ScannerTokenFixture {
  TokenTestCase *testCase;
};

TokenTestCase tokenTestCases[] = {
    {.source = "(", .type = TOKEN_LEFT_PAREN, .len = 1},
    {.source = "+", .type = TOKEN_PLUS, .len = 1},
    {.source = "-", .type = TOKEN_MINUS, .len = 1},
    {.source = "*", .type = TOKEN_STAR, .len = 1},
    {.source = "/", .type = TOKEN_SLASH, .len = 1},
    {.source = "123.45", .type = TOKEN_NUMBER, .len = 6}};

UTEST_I_SETUP(ScannerTokenFixture) {
  utest_fixture->testCase = &tokenTestCases[utest_index];
  ASSERT_EQ(ARRAY_SIZE(tokenTestCases), 6u);
}

UTEST_I_TEARDOWN(ScannerTokenFixture) {
  utest_fixture->testCase = NULL;
  ASSERT_LE(0u, utest_index);
}

UTEST_I(ScannerTokenFixture, scanToken, 6) {
  TokenTestCase testCase = *utest_fixture->testCase;

  Scanner scanner;
  initScanner(&scanner, testCase.source);

  Token token = scanToken(&scanner);

  ASSERT_EQ(token.length, testCase.len);
  ASSERT_EQ(token.line, 1);
  ASSERT_STREQ(token.lexeme, testCase.source);
  ASSERT_EQ(token.type, testCase.type);
}

typedef struct WhitespaceTestCase {
  char *source;
  int lines;
} WhitespaceTestCase;

typedef struct ScannerWhitespaceFixture {
  WhitespaceTestCase *testCase;
} ScannerWhitespaceFixture;

WhitespaceTestCase whitespaceTestCases[] = {
    {.source = "  \n", .lines = 2},
    {.source = "\r\n", .lines = 2},
    {.source = "\n\n", .lines = 3},
    {.source = "\t\n", .lines = 2},
    {.source = "// Inline comment\n", .lines = 2}};

UTEST_I_SETUP(ScannerWhitespaceFixture) {
  utest_fixture->testCase = &whitespaceTestCases[utest_index];
  ASSERT_EQ(ARRAY_SIZE(whitespaceTestCases), 5u);
}

UTEST_I_TEARDOWN(ScannerWhitespaceFixture) {
  utest_fixture->testCase = NULL;
  ASSERT_LE(0u, utest_index);
}

UTEST_I(ScannerWhitespaceFixture, scanToken, 5) {
  WhitespaceTestCase testCase = *utest_fixture->testCase;

  Scanner scanner;
  initScanner(&scanner, testCase.source);

  scanToken(&scanner);

  ASSERT_EQ(scanner.line, testCase.lines);
}
