#ifndef HYDRO_SCANNER_H
#define HYDRO_SCANNER_H

#include "token.h"

typedef struct Scanner {
  char *start;   // Points to start of the current lexeme in the source.
  char *current; // Points to current character in the source to be scanned.
  int line;      // Current line the scanner is in the source.
} Scanner;

void initScanner(Scanner *scanner, char *source);
Token scanToken(Scanner *scanner);

#endif
