#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdint.h>


const char* KEYWORD_IF;
const char* KEYWORD_ELSE;
const char* KEYWORD_DO;
const char* KEYWORD_WHILE;
const char* KEYWORD_FOR;
const char* KEYWORD_SWITCH;
const char* KEYWORD_CASE;
const char* KEYWORD_CONTINUE;
const char* KEYWORD_BREAK;
const char* KEYWORD_RETURN;
const char* KEYWORD_TRUE;
const char* KEYWORD_FALSE;
const char* KEYWORD_BOOL;
const char* KEYWORD_INT;
const char* KEYWORD_VAR;
const char* KEYWORD_CONST;
const char* KEYWORD_FUNC;
const char* KEYWORD_STRUCT;


typedef enum TokenKind {
  TOKEN_EOF = '\0',
  TOKEN_INT = 128,
  TOKEN_NAME,
  TOKEN_KEYWORD,
  TOKEN_OPERATOR,
  TOKEN_SEPARATOR,
  TOKEN_ERROR = 255,
} TokenKind;


typedef struct Token {
  const char*   file;
  int           line;
  int           pos;
  TokenKind     kind;
  const char*   start;
  const char*   end;
  union {
    uint64_t    value;
    char*       name;
    char*       optype;
    char*       message;
  };
} Token;


typedef struct Lexer {
  const char* file;
  const char* stream;
  int         currentLine;
  int         currentPosition;
} Lexer;


Lexer newLexer(const char* file, const char* stream);

Token nextToken(Lexer* lexer);


#endif  // __LEXER_H__
