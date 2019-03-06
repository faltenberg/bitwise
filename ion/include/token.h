#ifndef __TOKEN_H__
#define __TOKEN_H__


/**
 * Tokens
 * ======
 *
 * Source code is divided into a stream of tokens. Each `Token` is one of many `TokenKind`s.
 * Depending on the kind tokens may have different fields that hold additional information.
 * The details are described later. Each token also stores the file name and the loacation it
 * starts and ends. There exist a supporting factory function `createTokenXYZ()` for each major
 * `TokenKind`.
 *
 *
 * `struct TokenLoc`
 * ----------------------
 *
 * For better error messages each token holds the start and end location of its encounter.
 *
 * ```c {.line-numbers}
 * #include "token.h"
 * #include <stdio.h>
 *
 * extern Token readToken();
 *
 * int main() {
 *  Token t = readToken();  // read a token from a source file
 *  printf("%s:%d:%d Token[%.*s]\n",
 *         t.fileName, t.start.line, t.start.pos, t.end.ptr - t.start.ptr, t.start.ptr);
 * }
 * ```
 *
 *
 * `struct Token`
 * --------------
 *
 * `Token`s are differentiated between various major `TokenKind`s. Keyword, operator and separator
 * tokens have a subkind, that differentiates the token further. All the minor `TokenKind`s are
 * only used as subkinds.
 *
 * ```c {.line-numbers}
 * struct Token {
 *   TokenKind kind = TOKEN_EOF;
 *   char*     fileName;
 *   TokenLoc  start;
 * };
 *
 * struct Token {
 *   TokenKind kind = TOKEN_INT;
 *   char*     fileName;
 *   TokenLoc  start;
 *   TokenLoc  end;
 *   uint64_t  value;
 * };
 *
 * struct Token {
 *   TokenKind kind = TOKEN_NAME;
 *   char*     fileName;
 *   TokenLoc  start;
 *   TokenLoc  end;
 *   char*     name;
 * };
 *
 * struct Token {
 *   TokenKind kind = TOKEN_KEYWORD | TOKEN_OPERATOR | TOKEN_SEPARATOR;
 *   char*     fileName;
 *   TokenLoc  start;
 *   TokenLoc  end;
 *   TokenKind subKind;
 * };
 *
 * struct Token {
 *   TokenKind kind = TOKEN_COMMENT;
 *   char*     fileName;
 *   TokenLoc  start;
 *   TokenLoc  end;
 * };
 *
 * struct Token {
 *   TokenKind kind = TOKEN_ERROR;
 *   char*     fileName;
 *   TokenLoc  start;
 *   TokenLoc  end;
 *   char*     message;
 * };
 * ```
 */


#include <stdint.h>


typedef enum TokenKind {
  TOKEN_EOF,
  TOKEN_INT,
  TOKEN_NAME,
  TOKEN_KEYWORD,
  TOKEN_OPERATOR,
  TOKEN_SEPARATOR,
  TOKEN_COMMENT,
  TOKEN_ERROR,

  TOKEN_KEYWORD_IF,
  TOKEN_KEYWORD_ELSE,
  TOKEN_KEYWORD_DO,
  TOKEN_KEYWORD_WHILE,
  TOKEN_KEYWORD_FOR,
  TOKEN_KEYWORD_SWITCH,
  TOKEN_KEYWORD_CASE,
  TOKEN_KEYWORD_BREAK,
  TOKEN_KEYWORD_CONTINUE,
  TOKEN_KEYWORD_RETURN,
  TOKEN_KEYWORD_TRUE,
  TOKEN_KEYWORD_FALSE,
  TOKEN_KEYWORD_VAR,
  TOKEN_KEYWORD_CONST,
  TOKEN_KEYWORD_FUNC,
  TOKEN_KEYWORD_STRUCT,

  TOKEN_OP_PLUS,
  TOKEN_OP_MINUS,
  TOKEN_OP_ADD,
  TOKEN_OP_INC,
  TOKEN_OP_SUB,
  TOKEN_OP_DEC,
  TOKEN_OP_MUL,
  TOKEN_OP_DIV,
  TOKEN_OP_MOD,
  TOKEN_OP_CMP_LT,
  TOKEN_OP_CMP_LEQ,
  TOKEN_OP_CMP_GT,
  TOKEN_OP_CMP_GEQ,
  TOKEN_OP_CMP_EQ,
  TOKEN_OP_CMP_NEQ,
  TOKEN_OP_LSL,
  TOKEN_OP_LSR,
  TOKEN_OP_ASR,
  TOKEN_OP_AND,
  TOKEN_OP_ANDAND,
  TOKEN_OP_OR,
  TOKEN_OP_OROR,
  TOKEN_OP_XOR,
  TOKEN_OP_NOT,
  TOKEN_OP_NEG,

  TOKEN_ASSIGN,
  TOKEN_COLONASSIGN,
  TOKEN_ASSIGN_ADD,
  TOKEN_ASSIGN_SUB,
  TOKEN_ASSIGN_MUL,
  TOKEN_ASSIGN_DIV,
  TOKEN_ASSIGN_MOD,
  TOKEN_ASSIGN_LSL,
  TOKEN_ASSIGN_LSR,
  TOKEN_ASSIGN_ASR,
  TOKEN_ASSIGN_AND,
  TOKEN_ASSIGN_OR,
  TOKEN_ASSIGN_XOR,
  TOKEN_ASSIGN_NOT,
  TOKEN_ASSIGN_NEG,

  TOKEN_SEP_LPAREN,
  TOKEN_SEP_RPAREN,
  TOKEN_SEP_LBRACKET,
  TOKEN_SEP_RBRACKET,
  TOKEN_SEP_LBRACE,
  TOKEN_SEP_RBRACE,
  TOKEN_SEP_DOT,
  TOKEN_SEP_DOTDOT,
  TOKEN_SEP_COMMA,
  TOKEN_SEP_SEMICOLON,
  TOKEN_SEP_COLON,
  TOKEN_SEP_COLONCOLON,
  TOKEN_SEP_ARROW,
} TokenKind;


typedef struct TokenLoc {
  int   line;
  int   pos;
  char* ptr;
} TokenLoc;


typedef struct Token {
  TokenKind kind;
  char*     fileName;
  TokenLoc  start;
  TokenLoc  end;
  union {
    TokenKind subKind;
    uint64_t value;
    char*    name;
    char*    message;
  };
} Token;


Token createTokenEOF(char* fileName, TokenLoc start);

Token createTokenInt(char* fileName, TokenLoc start, TokenLoc end, uint64_t value);

Token createTokenName(char* fileName, TokenLoc start, TokenLoc end, char* name);

Token createTokenKeyword(char* fileName, TokenLoc start, TokenLoc end, TokenKind keyword);

Token createTokenOperator(char* fileName, TokenLoc start, TokenLoc end, TokenKind operator);

Token createTokenSeparator(char* fileName, TokenLoc start, TokenLoc end, TokenKind separator);

Token createTokenComment(char* fileName, TokenLoc start, TokenLoc end);

Token createTokenError(char* fileName, TokenLoc start, TokenLoc end, char* message);


#endif  // __TOKEN_H__
