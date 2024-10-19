#ifndef HYDRO_SCANNER_H
#define HYDRO_SCANNER_H

#include "token.h"

typedef struct Scanner {
  const char *start;   // Points to starting character of token to be scanned.
  const char *current; // Points to current character in source to be scanned.
  int line;            // Current line the scanner is in the source.
} Scanner;

void initScanner(Scanner *scanner, const char *source);
Token scanToken(Scanner *scanner);

#endif
