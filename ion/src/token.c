#include "token.h"


static Token createToken(TokenKind kind, char* fileName, TokenLoc start, TokenLoc end) {
  return (Token){ .kind=kind, .fileName=fileName, .start=start, .end=end };
}


Token createTokenEOF(char* fileName, TokenLoc start) {
  return createToken(TOKEN_EOF, fileName, start, start);
}


Token createTokenInt(char* fileName, TokenLoc start, TokenLoc end, uint64_t value) {
  Token token = createToken(TOKEN_INT, fileName, start, end);
  token.value = value;
  return token;
}


Token createTokenName(char* fileName, TokenLoc start, TokenLoc end, char* name) {
  Token token = createToken(TOKEN_NAME, fileName, start, end);
  token.name = name;
  return token;
}


Token createTokenKeyword(char* fileName, TokenLoc start, TokenLoc end, TokenKind keyword) {
  Token token = createToken(TOKEN_KEYWORD, fileName, start, end);
  token.subKind = keyword;
  return token;
}


Token createTokenOperator(char* fileName, TokenLoc start, TokenLoc end, TokenKind operator) {
  Token token = createToken(TOKEN_OPERATOR, fileName, start, end);
  token.subKind = operator;
  return token;
}


Token createTokenSeparator(char* fileName, TokenLoc start, TokenLoc end, TokenKind separator) {
  Token token = createToken(TOKEN_SEPARATOR, fileName, start, end);
  token.subKind = separator;
  return token;
}


Token createTokenComment(char* fileName, TokenLoc start, TokenLoc end) {
  return createToken(TOKEN_COMMENT, fileName, start, end);
}


Token createTokenError(char* fileName, TokenLoc start, TokenLoc end, char* message) {
  Token token = createToken(TOKEN_ERROR, fileName, start, end);
  token.message = message;
  return token;
}
