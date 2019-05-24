#ifndef __LEXER_H__
#define __LEXER_H__

#include "source.h"
#include "token.h"


typedef struct Lexer {
  Source   source;
  TokenLoc currentLoc;
  TokenLoc nextLoc;
  int      index;
  char     currentChar;
} Lexer;


Lexer lexerFromSource(Source src);

Token nextToken(Lexer* lexer);

Token peekToken(Lexer* lexer);


#endif  // __LEXER_H__
