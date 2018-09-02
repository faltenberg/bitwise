#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdint.h>


typedef enum TokenKind {
  TOKEN_INT = 128,
  TOKEN_NAME,
} TokenKind;


typedef struct Token {
  TokenKind kind;
  union {
    uint64_t value;
    struct {
      const char* start;
      const char* end;
    };
  };
} Token;


#endif  // __LEXER_H__
