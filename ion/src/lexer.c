#include "lexer.h"
#include "sbuffer.h"
#include "strintern.h"

#include <ctype.h>
#include <stdbool.h>


static const char* _keywordIf;
static const char* _keywordElse;
static const char* _keywordFor;
static const char* _keywordWhile;


static void init() {
  _keywordIf    = strintern("if");
  _keywordElse  = strintern("else");
  _keywordFor   = strintern("for");
  _keywordWhile = strintern("while");
}


static bool isKeyword(const char* token) {
  return token == _keywordIf  || token == _keywordElse  ||
         token == _keywordFor || token == _keywordWhile;
}


Lexer newLexer(const char* stream) {
  init();
  return (Lexer) { .stream=stream };
}


Token nextToken(Lexer* lexer) {
  Token token = {};

  while (*lexer->stream == ' ') {
    lexer->stream++;
  }

  token.start = lexer->stream;
  switch (*lexer->stream) {
    case '0':  case '1':  case '2':  case '3':  case '4':
    case '5':  case '6':  case '7':  case '8':  case '9':
      do {
        token.value *= 10;
        token.value += *lexer->stream - '0';
        lexer->stream++;
      } while (isdigit(*lexer->stream));
      token.kind = TOKEN_INT;
      break;

    case 'a':  case 'b':  case 'c':  case 'd':  case 'e':  case 'f':  case 'g':
    case 'h':  case 'i':  case 'j':  case 'k':  case 'l':  case 'm':  case 'n':
    case 'o':  case 'p':  case 'q':  case 'r':  case 's':  case 't':  case 'u':
    case 'v':  case 'w':  case 'x':  case 'y':  case 'z':
    case 'A':  case 'B':  case 'C':  case 'D':  case 'E':  case 'F':  case 'G':
    case 'H':  case 'I':  case 'J':  case 'K':  case 'L':  case 'M':  case 'N':
    case 'O':  case 'P':  case 'Q':  case 'R':  case 'S':  case 'T':  case 'U':
    case 'V':  case 'W':  case 'X':  case 'Y':  case 'Z':
    case '_':
      do {
        lexer->stream++;
      } while (isalnum(*lexer->stream) || *lexer->stream == '_');
      token.name = strinternRange(token.start, lexer->stream);
      token.kind = isKeyword(token.name) ? TOKEN_KEYWORD : TOKEN_NAME;
      break;
/*
    case '+':  case '-':  case '*':  case '/':  case '%':
    case '&':  case '|':  case '^':  case '!':  case '~':  case '=':
    case '.':  case ',':  case ':':  case ';':  case '<':  case '>':
    case '(':  case ')':  case '[':  case ']':  case '{':  case '}':
      lexer->stream++;
      token.kind = TOKEN_OPERATOR;
      break;
*/
    default:
      token.kind = *lexer->stream++;
      break;
  }
  token.end = lexer->stream;

  return token;
}
