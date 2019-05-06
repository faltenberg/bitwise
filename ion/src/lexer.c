#include "lexer.h"

#include <stdio.h>


Lexer newLexer(Source src) {
  return (Lexer){ };
}


static Token tokenEOF() {
  return (Token){ .kind=TOKEN_EOF, .content="\0" };
}


static Token tokenInt(const char* value) {
  return (Token){ .kind=TOKEN_INT, .content=value };
}


static Token tokenName(const char* name) {
  return (Token){ .kind=TOKEN_NAME, .content=name };
}


static Token tokenOp(const char* op) {
  return (Token){ .kind=TOKEN_OP, .content=op };
}


static Token tokenSep(const char* sep) {
  return (Token){ .kind=TOKEN_SEP, .content=sep };
}


static int    current = -1;
static Token* tokens;
static int    length;
static bool   initialized = false;

static void init() {
  if (!initialized) {
    length = 7;
    tokens = (Token*) malloc(length*sizeof(Token));
    tokens[0] = tokenName("a");
    tokens[1] = tokenOp("*");
    tokens[2] = tokenName("b");
    tokens[3] = tokenOp("+");
    tokens[4] = tokenName("c");
//    tokens[5] = tokenOp("*");
    tokens[5] = tokenName("d");
    tokens[6] = tokenEOF();
    initialized = true;
  }
}


Token nextToken(Lexer* lexer) {
  init();
  if (current < length-1) {
    current++;
  }
  return tokens[current];
}


Token peekToken(Lexer* lexer) {
  init();
  return tokens[current+1];
}


void printToken(const Token* token) {
  switch (token->kind) {
    case TOKEN_NONE:
      printf("Token[ TOKEN_NONE ]\n");
      break;
    case TOKEN_EOF:
      printf("Token[ TOKEN_EOF ]\n");
      break;
    case TOKEN_INT:
      printf("Token[ TOKEN_INT %s ]\n", token->content);
      break;
    case TOKEN_NAME:
      printf("Token[ TOKEN_NAME %s ]\n", token->content);
      break;
    case TOKEN_OP:
      printf("Token[ TOKEN_OP %s ]\n", token->content);
      break;
    case TOKEN_SEP:
      printf("Token[ TOKEN_SEP %s ]\n", token->content);
      break;
    default:
      printf("Token[ UNKNOWN ]\n");
      break;
  }
}
