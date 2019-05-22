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
    case TOKEN_OP:
      return stringFromArray("TOKEN_OP");
    case TOKEN_SEP:
      return stringFromArray("TOKEN_SEP");
  }
}


bool isKeyword(string s) {
  return cstrequal(s, "if") ||
         cstrequal(s, "else") ||
         cstrequal(s, "do") ||
         cstrequal(s, "while") ||
         cstrequal(s, "for") ||
         cstrequal(s, "switch") ||
         cstrequal(s, "case") ||
         cstrequal(s, "break") ||
         cstrequal(s, "continue") ||
         cstrequal(s, "return") ||
         cstrequal(s, "true") ||
         cstrequal(s, "false") ||
         cstrequal(s, "var") ||
         cstrequal(s, "const") ||
         cstrequal(s, "func") ||
         cstrequal(s, "struct") ||
         cstrequal(s, "_");
}
