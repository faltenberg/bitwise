#include "lexer.h"
#include "strintern.h"

#include <ctype.h>
#include <stdbool.h>


static void init() {
  KEYWORD_IF       = strintern("if");
  KEYWORD_ELSE     = strintern("else");
  KEYWORD_DO       = strintern("do");
  KEYWORD_WHILE    = strintern("while");
  KEYWORD_FOR      = strintern("for");
  KEYWORD_SWITCH   = strintern("switch");
  KEYWORD_CASE     = strintern("case");
  KEYWORD_CONTINUE = strintern("continue");
  KEYWORD_BREAK    = strintern("break");
  KEYWORD_RETURN   = strintern("return");
  KEYWORD_TRUE     = strintern("true");
  KEYWORD_FALSE    = strintern("false");
  KEYWORD_BOOL     = strintern("bool");
  KEYWORD_INT      = strintern("int");
  KEYWORD_VAR      = strintern("var");
  KEYWORD_CONST    = strintern("const");
  KEYWORD_FUNC     = strintern("func");
  KEYWORD_STRUCT   = strintern("struct");
}


static bool isKeyword(const char* token) {
  return token == KEYWORD_IF ||
         token == KEYWORD_ELSE ||
         token == KEYWORD_DO ||
         token == KEYWORD_WHILE ||
         token == KEYWORD_FOR ||
         token == KEYWORD_SWITCH ||
         token == KEYWORD_CASE ||
         token == KEYWORD_CONTINUE ||
         token == KEYWORD_BREAK ||
         token == KEYWORD_RETURN ||
         token == KEYWORD_TRUE ||
         token == KEYWORD_FALSE ||
         token == KEYWORD_BOOL ||
         token == KEYWORD_INT ||
         token == KEYWORD_VAR ||
         token == KEYWORD_CONST ||
         token == KEYWORD_FUNC ||
         token == KEYWORD_STRUCT;
}


Lexer newLexer(const char* stream) {
  init();
  return (Lexer){ .stream=stream };
}


Token nextToken(Lexer* lexer) {
  Token token = {};

  while (*lexer->stream == ' ') {
    lexer->stream++;
  }

  token.start = lexer->stream;
  switch (*lexer->stream) {
    case '\0':
      lexer->stream++;
      token.kind = TOKEN_EOF;
      break;

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

    case '+':  case '&':  case '|':  case ':':
      lexer->stream++;
      if (*lexer->stream == *(lexer->stream-1) || *lexer->stream == '=') {
        lexer->stream++;
      }
      token.kind = TOKEN_OPERATOR;
      token.optype = strinternRange(token.start, lexer->stream);
      break;

    case '-':
      lexer->stream++;
      if (*lexer->stream == *(lexer->stream-1) || *lexer->stream == '=' || *lexer->stream == '>') {
        lexer->stream++;
      }
      token.kind = TOKEN_OPERATOR;
      token.optype = strinternRange(token.start, lexer->stream);
      break;

    case '*':  case '/':  case '%':  case '^':  case '~':  case '!':  case '=':
      lexer->stream++;
      if (*lexer->stream == '=') {
        lexer->stream++;
      }
      token.kind = TOKEN_OPERATOR;
      token.optype = strinternRange(token.start, lexer->stream);
      break;

    case '<':  case '>':
      lexer->stream++;
      if (*lexer->stream == *(lexer->stream-1) || *lexer->stream == '=') {
        lexer->stream++;
        if (*lexer->stream == *(lexer->stream-2) || *lexer->stream == '=') {
          lexer->stream++;
          if (*lexer->stream == '=') {
            lexer->stream++;
          }
        }
      }
      token.kind = TOKEN_OPERATOR;
      token.optype = strinternRange(token.start, lexer->stream);
      break;

    case '.':
      lexer->stream++;
      if (*lexer->stream == *(lexer->stream-1)) {
        lexer->stream++;
      }
      token.kind = TOKEN_OPERATOR;
      token.optype = strinternRange(token.start, lexer->stream);
      break;

    case ',':  case ';':  case '(':  case ')':  case '[':  case ']':  case '{':  case '}':
      lexer->stream++;
      token.kind = TOKEN_SEPARATOR;
      token.optype = strinternRange(token.start, lexer->stream);
      break;

    default:
      lexer->stream++;
      token.kind = TOKEN_ERROR;
      token.message = strintern("unknown character");
      break;
  }
  token.end = lexer->stream;

  return token;
}
