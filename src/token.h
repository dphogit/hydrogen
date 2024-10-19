#ifndef HYDRO_TOKEN_H
#define HYDRO_TOKEN_H

typedef enum {
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_NUMBER,
  TOKEN_ERROR,
  TOKEN_EOF,
} TokenType;

typedef struct Token {
  TokenType type;
  const char *lexeme;
  int length;
  int line;
} Token;

#endif
