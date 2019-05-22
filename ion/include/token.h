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
 * #include "lexer.h"
 * #include <assert.h>
 * #include <stdio.h>
 *
 * int main() {
 *   // create a source for tokens
 *   Lexer lexer = createLexer(sourceFromString("x + 42; 1x"));
 *
 *   Token t = nextToken(&lexer);
 *   assert(t.kind != TOKEN_NONE);  // TOKEN_NONE is an invalid token
 *   assert(t.kind == TOKEN_NAME);
 *   // tokens have a start and end location within the source
 *   // start and end is the occurance of the first and the last character of the token
 *   assert(t.start.line == 1);
 *   assert(t.start.pos == 1);
 *   assert(t.end.line == 1);
 *   assert(t.end.pos == 1);
 *   assert(cstrequal(t.source->fileName, "<cstring>"));
 *   assert(cstrequal(t.chars, "x"));  // get token characters
 *
 *   t = nextToken(&lexer);
 *   assert(t.kind == TOKEN_OP);
 *   assert(t.start.pos == 3);
 *   assert(t.end.pos == 3);
 *   assert(cstrequal(t.chars, "+"));
 *
 *   t = nextToken(&lexer);
 *   assert(t.kind == TOKEN_INT);
 *   assert(t.start.pos == 5);
 *   assert(t.end.pos == 6);
 *   assert(cstrequal(t.chars, "42"));
 *
 *   t = nextToken(&lexer);
 *   assert(t.kind == TOKEN_SEP);
 *   assert(t.start.pos == 7);
 *   assert(t.end.pos == 7);
 *   assert(cstrequal(t.chars, ";"));
 *
 *   t = nextToken(&lexer);
 *   assert(t.kind == TOKEN_ERROR);
 *   assert(t.start.pos == 9);
 *   assert(t.end.pos == 10);
 *   string line = getLine(t.source, t.start.line);
 *   printf("%.*s:%d:%d: error in \"%.*s\": %.*s",
 *          t.source->fileName.len, t.source->fileName.chars, t.start.line, t.start.pos,
 *          line.len, line.chars, t.chars.len, t.chars.chars);
 *
 *   t = nextToken(&lexer);
 *   assert(t.kind == TOKEN_EOF);
 *   assert(t.start.pos == 11);
 *   assert(t.end.pos == 11);
 *   assert(cstrequal(t.chars, "\0"));
 * }
 * ```
 */


#import "source.h"
#import "str.h"


typedef enum TokenKind {
  TOKEN_NONE,
  TOKEN_EOF,
  TOKEN_ERROR,
  TOKEN_COMMENT,
  TOKEN_INT,
  TOKEN_NAME,
  TOKEN_KEYWORD,
  TOKEN_OP,
  TOKEN_SEP,
} TokenKind;


string str(TokenKind kind);

bool isKeyword(string s);


typedef struct TokenLoc {
  int line;
  int pos;
} TokenLoc;


typedef struct Token {
  TokenKind kind;
  Source*   source;
  TokenLoc  start;
  TokenLoc  end;
  string    chars;
} Token;


#endif  // __TOKEN_H__
