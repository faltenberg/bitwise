#include "lexer.h"
#include "sbuffer.h"

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>


char* stream;
Token token;


void parseInteger() {
  uint64_t value = 0;
  while (isdigit(*stream)) {
    value *= 10;
    value += *stream - '0';
    stream++;
  }
  token.kind = TOKEN_INT;
  token.value = value;
}


void parseIdentifier() {
  token.start = stream++;
  while (isalnum(*stream) || *stream == '_') {
    stream++;
  }
  token.kind = TOKEN_NAME;
  token.end = stream;
}


void nextToken() {
  switch (*stream) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      parseInteger();
      break;

    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
    case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
    case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
    case 'v': case 'w': case 'x': case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
    case 'V': case 'W': case 'X': case 'Y': case 'Z':
    case '_':
      parseIdentifier();
      break;

    default:
      token.kind = *stream++;
      break;
  }
}


void printToken(Token token) {
  printf("Token[ ");
  switch (token.kind) {
    case TOKEN_INT:
      printf("%s(%lu)", "TOKEN_INT", token.value);
      break;

    case TOKEN_NAME:
      printf("%s(%.*s)", "TOKEN_NAME", (int) (token.end - token.start), token.start);
      break;

    default:
      printf("kind: %c", token.kind);
  }
  printf(" ]\n");
}


int main(int argc, char** argv) {
  stream = "var+1234";
  printf("Stream: %s\n", stream);
  do {
    nextToken();
    printToken(token);
  } while (token.kind);
  return 0;
}
