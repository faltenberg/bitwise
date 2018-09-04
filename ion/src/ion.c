#include "lexer.h"
#include "sbuffer.h"

#include <stdio.h>


void printToken(const Token* token) {
  printf("Token[ ");
  switch (token->kind) {
    case TOKEN_INT:
      printf("%s(%lu)", "TOKEN_INT", token->value);
      break;

    case TOKEN_NAME:
      printf("%s(%.*s)", "TOKEN_NAME", (int) (token->end - token->start), token->start);
      break;

    default:
      printf("kind: '%c'", token->kind);
  }
  printf(" ]\n");
}


int main(int argc, char** argv) {
  Lexer lexer = newLexer("123 + ab_12[12_ab]");
  printf("Stream: %s\n", lexer.stream);
  Token* tokens = NULL;

  Token token;
  do {
    token = nextToken(&lexer);
    bufPush(tokens, token);
  } while (token.kind);

  for (int i = 0; i < bufLength(tokens); i++) {
    printToken(&tokens[i]);
  }

  bufFree(tokens);
  return 0;
}
