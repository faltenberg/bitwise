#include "token.h"


string str(TokenKind kind) {
  switch (kind) {
    case TOKEN_NONE:
      return stringFromArray("TOKEN_NONE");
    case TOKEN_EOF:
      return stringFromArray("TOKEN_EOF");
    case TOKEN_ERROR:
      return stringFromArray("TOKEN_ERROR");
    case TOKEN_COMMENT:
      return stringFromArray("TOKEN_COMMENT");
    case TOKEN_INT:
      return stringFromArray("TOKEN_INT");
    case TOKEN_NAME:
      return stringFromArray("TOKEN_NAME");
    case TOKEN_KEYWORD:
      return stringFromArray("TOKEN_KEYWORD");
    case TOKEN_SYMBOL:
      return stringFromArray("TOKEN_SYMBOL");
  }
}
