#ifndef HYDRO_COMPILER_H
#define HYDRO_COMPILER_H

#include <stdbool.h>

#include "chunk.h"
#include "token.h"

typedef struct Parser {
  Token *start;
  Token *current;
} Parser;

bool compile(const char *source, Chunk *chunk);

#endif
