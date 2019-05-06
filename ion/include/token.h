#ifndef __TOKEN_H__
#define __TOKEN_H__


/**
 * Tokens
 * ======
 *
 * Source code is divided into a stream of `Token`s by the lexer. `Tokens` with similar properties
 * belong to the same `TokenKind` such as numbers and identiers. For each token additional
 * information is stored, namely the position of the first and last character of the token. The raw
 * token characters are stored, too.
 *
 *
 * Example
 * -------
 *
 * ```c {.line-numbers}
 * #include "token.h"
 * #include <assert.h>
 *
 * Token readToken() {
 *   return (Token){};  // fill it with data or use the lexer
 * }
 *
 * int main() {
 *   Lexer lexer = createLexer(fromString(""));
 *   Token t = readToken();
 *
 *   assert(t.kind != TOKEN_EOF);
 *   assert(t.kind != TOKEN_INT);
 *   assert(t.kind != TOKEN_NAME);
 *   assert(t.kind != TOKEN_OP);
 *   assert(t.kind != TOKEN_SEP);
 *   assert(t.kind == TOKEN_NONE);
 *
 *   assert(t.start.line == 0);
 *   assert(t.start.pos == 0);
 *   assert(t.end.line == 0);
 *   assert(t.end.pos == 0);
 *
 *   assert(t.src->fileName == "");
 *   assert(t.chars == "");
 * }
 * ```
 */


#import "source.h"
#import "str.h"


typedef enum TokenKind {
  TOKEN_NONE,
  TOKEN_EOF,
  TOKEN_INT,
  TOKEN_NAME,
  TOKEN_OP,
  TOKEN_SEP,
} TokenKind;


typedef struct TokenLoc {
  int     line;
  int     pos;
} TokenLoc;


typedef struct Token {
  TokenKind     kind;
  const Source* src;
  TokenLoc      start;
  TokenLoc      end;
  string        chars;
} Token;


void printToken(const Token* token);


#endif  // __TOKEN_H__
