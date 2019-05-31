#ifndef __TOKEN_H__
#define __TOKEN_H__


/**
 * Tokens
 * ======
 *
 * Source code is divided into a stream of `Token`s by the lexer. `Tokens` with similar properties
 * belong to the same `TokenKind` such as numbers and identiers. For each token the locations of
 * their first and last character within the source. Each token contains a string with the token
 * characters, which is a window into the source code. If the lexer detects some grammar violation
 * it will return a `TOKEN_ERROR` token describing the error. The token owns the error. Since the
 * error has allocated memory for the message, it must be freed!
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
 *   Source src = sourceFromString("x + 42; 1x // error\n"));
 *   Lexer lexer = lexerFromSource(&src);
 *
 *   Token token = nextToken(&lexer);
 *   assert(token.kind != TOKEN_NONE);  // TOKEN_NONE is an invalid token
 *   assert(token.kind == TOKEN_NAME);
 *   // tokens have a start and end location within the source
 *   // start and end is the occurance of the first and the last character of the token
 *   assert(token.start.line == 1);
 *   assert(token.start.pos == 1);
 *   assert(token.end.line == 1);
 *   assert(token.end.pos == 1);
 *   assert(cstrequal(token.source->fileName, "<cstring>"));
 *   assert(cstrequal(token.chars, "x"));  // get token characters
 *
 *   token = nextToken(&lexer);
 *   assert(token.kind == TOKEN_SYMBOL);
 *   assert(token.start.pos == 3);
 *   assert(token.end.pos == 3);
 *   assert(cstrequal(token.chars, "+"));
 *
 *   token = nextToken(&lexer);
 *   assert(token.kind == TOKEN_INT);
 *   assert(token.start.pos == 5);
 *   assert(token.end.pos == 6);
 *   assert(cstrequal(token.chars, "42"));
 *   printf("%.*s", token.chars.len, token.chars.chars);
 *
 *   token = nextToken(&lexer);
 *   assert(token.kind == TOKEN_SYMBOL);
 *   assert(token.start.pos == 7);
 *   assert(token.end.pos == 7);
 *   assert(cstrequal(token.chars, ";"));
 *
 *   token = nextToken(&lexer);
 *   assert(token.kind == TOKEN_ERROR);
 *   assert(token.start.pos == 9);
 *   assert(token.end.pos == 10);
 *   printf("%.*s", token.error->message.len, token.error->message.chars);
 *   deleteError(token.error);  // must be freed
 *
 *   // comments are tokens as well (note that \n is not part of the comment)
 *   token = nextToken(&lexer);
 *   assert(token.kind == TOKEN_COMMENT);
 *   assert(token.start.pos == 12);
 *   assert(token.end.pos == 19);
 *   assert(cstrequal(token.chars, "// error"));
 *
 *   token = nextToken(&lexer);
 *   assert(token.kind == TOKEN_EOF);
 *   assert(token.start.line == 2);
 *   assert(token.start.pos == 1);
 *   assert(token.end.pos == 1);
 *   assert(cstrequal(token.chars, ""));
 *
 *   deleteSource(&src);
 * }
 * ```
 */


#include "source.h"
#include "str.h"
#include "loc.h"
#include "error.h"

#include <stddef.h>


/**
 * `TokenKind` differentiates various token categories.
 *
 * - **enum:** `TOKEN_NONE`    - invalid token, no token at all
 * - **enum:** `TOKEN_EOF`     - final token returned when all the source was processed
 * - **enum:** `TOKEN_ERROR`   - error token due to violation of the lexical grammar
 * - **enum:** `TOKEN_COMMENT` - single-line or multi-line comment token
 * - **enum:** `TOKEN_INT`     - integer literal token
 * - **enum:** `TOKEN_NAME`    - identifier token
 * - **enum:** `TOKEN_KEYWORD` - keyword token like "if", "else", etc.
 * - **enum:** `TOKEN_SYMBOL`  - special characters like "+", "++", "<=", "(", "->", etc.
 */
typedef enum TokenKind {
  TOKEN_NONE,
  TOKEN_EOF,
  TOKEN_ERROR,
  TOKEN_COMMENT,
  TOKEN_INT,
  TOKEN_NAME,
  TOKEN_KEYWORD,
  TOKEN_SYMBOL,
} TokenKind;


/**
 * `str()` returns the `TokenKind` in a string format.
 *
 * - **param:** `kind` - the token kind
 * - **return:** the string representation of the token kind
 */
string str(TokenKind kind);


/**
 * `Token` is the smallest entity in a source file. Each token is defined by some regular
 * expression in some grammar. `Token` stores all the information that is necessary to distinguish
 * the token type and its position in the source as well as the underlying characters. The token
 * string must not be freed. Undefined behavior will occur if a token is used once the underlying
 * source was freed, since the string will point to some invalid memory.
 *
 * - **field:** `kind`   - the `TokenKind` of the token
 * - **field:** `source` - the pointer to the source the token was read from
 * - **field:** `start`  - the location of the token's first character within the source
 * - **field:** `end`    - the location of the token's last character within the source
 * - **field:** `chars`  - the string containing the token characters
 * - **field:** `error`  - the error with more information if token kind is `TOKEN_ERROR`
 */
typedef struct Token {
  TokenKind     kind;
  const Source* source;
  Location      start;
  Location      end;
  string        chars;
  Error*        error;
} Token;


#endif  // __TOKEN_H__
