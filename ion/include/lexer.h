#ifndef __LEXER_H__
#define __LEXER_H__


/**
 * Lexer
 * =====
 *
 * The `Lexer` transforms a source code to a stream of tokens according to some grammar. Once all
 * the source is processed, the lexer will return only `TOKEN_EOF` tokens henceforth without
 * consuming any characters, as there are none left. The lexer has a simple interface. Simply pass
 * it a source code and use `nexToken()` to retrieve tokens. For more information on the various
 * tokens, refer to *token.h*.
 *
 *
 * Example
 * -------
 *
 * ```c {.line-numbers}
 * #include "lexer.h"
 * #include <assert.h>
 * #include <stdio.h>
 *
 * int main() {
 *   Source src = sourceFromString("x = 2; $");
 *   Lexer lexer = lexerFromSource(&src);
 *
 *   Token token = nextToken(&lexer);
 *   assert(token.kind == TOKEN_NAME);
 *   assert(token.start.line == 1);
 *   assert(token.start.pos == 1);
 *
 *   token = nextToken(&lexer);
 *   assert(token.kind == TOKEN_SYMBOL);
 *   assert(token.start.line == 1);
 *   assert(token.start.pos == 3);
 *
 *   token = nextToken(&lexer);
 *   assert(token.kind == TOKEN_INT);
 *   assert(token.start.line == 1);
 *   assert(token.start.pos == 5);
 *
 *   token = nextToken(&lexer);
 *   assert(token.kind == TOKEN_SYMBOL);
 *   assert(token.start.line == 1);
 *   assert(token.start.pos == 6);
 *
 *   token = nextToken(&lexer);
 *   assert(token.kind == TOKEN_ERROR);
 *   assert(token.start.line == 1);
 *   assert(token.start.pos == 8);
 *   printf("%.*s", token.chars.len, token.chars.chars);
 *
 *   token = nextToken(&lexer);
 *   assert(token.kind == TOKEN_EOF);
 *   assert(token.start.line == 1);
 *   assert(token.start.pos == 9);
 *
 *   // finished, only TOKEN_EOF from now on
 *   token = nextToken(&lexer);
 *   assert(token.kind == TOKEN_EOF);
 *   assert(token.start.line == 1);
 *   assert(token.start.pos == 9);
 *
 *   deleteLexer(&lexer);
 *   deleteSource(&src);
 * }
 * ```
 */


#include "source.h"
#include "token.h"
#include "sbuffer.h"


/**
 * `Lexer` stores the relevant information to retrieve tokens from source code. The stored data is
 * meant to be used internally.
 *
 * - **field:** `source`      - the pointer to the source to read tokens from
 * - **field:** `index`       - the position of the current character
 * - **field:** `currentChar` - the current character
 * - **field:** `currentLoc`  - the location of the current token
 * - **field:** `nextLoc`     - the location of the next token used for counting lines and EOF
 * - **field:** `errorMsgs`   - a buffer to store error messages
 */
typedef struct Lexer {
  const Source* source;
  int           index;
  char          currentChar;
  Location      currentLoc;
  Location      nextLoc;
} Lexer;


/**
 * `lexerFromSource()` creates a new lexer for a source code.
 *
 * - **param:** `src` - the source to read tokens from
 * - **return:** the lexer for the source code
 */
Lexer lexerFromSource(const Source* src);


/**
 * `nextToken()` advances forward and returns the next token from the source code.
 *
 * - **param:** `lexer` - the lexer
 * - **return:** the next token
 */
Token nextToken(Lexer* lexer);


#endif  // __LEXER_H__
