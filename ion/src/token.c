#include "token.h"


#define CASE(e)  case e: return #e;


const char* strTokenKind(TokenKind kind) {
  switch (kind) {
    CASE(TOKEN_NONE);
    CASE(TOKEN_ERROR);
    CASE(TOKEN_EOF);
    CASE(TOKEN_COMMENT);
    CASE(TOKEN_INT);
    CASE(TOKEN_NAME);
    CASE(TOKEN_KEYWORD);
    CASE(TOKEN_SYMBOL);
  }
}
