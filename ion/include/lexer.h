#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdint.h>


typedef enum TokenKind {
  TOKEN_INT = 128,
  TOKEN_NAME,
  TOKEN_OPERATOR
} TokenKind;


typedef struct Token {
  TokenKind kind;
  const char* start;
  const char* end;
  union {
    uint64_t value;
  };
} Token;


typedef struct Lexer {
  const char* stream;
} Lexer;


Lexer newLexer(const char* stream);

Token nextToken(Lexer* lexer);


#endif  // __LEXER_H__
