#include "lexer.h"
#include "strintern.h"

#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


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


Lexer newLexer(const char* file, const char* stream) {
  init();
  return (Lexer){ .file=file, .stream=stream, .currentLine=1, .currentPosition=1 };
}


Token nextToken(Lexer* lexer) {
  Token token = (Token){ .file=lexer->file };

  while (*lexer->stream == ' ' || *lexer->stream == '\t' || *lexer->stream == '\n') {
    lexer->currentPosition++;
    if (*lexer->stream == '\n') {
      lexer->currentLine++;
      lexer->currentPosition = 1;
    }
    lexer->stream++;
  }

  token.start = lexer->stream;
  token.line = lexer->currentLine;
  token.pos = lexer->currentPosition;
  switch (*lexer->stream) {
    case '\0':
    {
      lexer->stream++;
      lexer->currentPosition++;
      token.kind = TOKEN_EOF;
    } break;

    case '0':  case '1':  case '2':  case '3':  case '4':
    case '5':  case '6':  case '7':  case '8':  case '9':
    {
      token.kind = TOKEN_INT;
      do {
        token.value *= 10;
        token.value += *lexer->stream - '0';
        lexer->stream++;
        lexer->currentPosition++;
      } while (isdigit(*lexer->stream));

      if (isalpha(*lexer->stream) || *lexer->stream == '_') {
        do {
          lexer->stream++;
          lexer->currentPosition++;
        } while (isalnum(*lexer->stream) || *lexer->stream == '_');
        token.kind = TOKEN_ERROR;
        token.message = (char*) malloc(strlen(token.file) + 4 + 2*sizeof(size_t)*8 +
                                       15 + lexer->stream - token.start + 1);
        sprintf(token.message, "%s:%d:%d  invalid number %.*s",
                token.file, token.line, token.pos, lexer->stream - token.start, token.start);
      }
    } break;

    case 'a':  case 'b':  case 'c':  case 'd':  case 'e':  case 'f':  case 'g':
    case 'h':  case 'i':  case 'j':  case 'k':  case 'l':  case 'm':  case 'n':
    case 'o':  case 'p':  case 'q':  case 'r':  case 's':  case 't':  case 'u':
    case 'v':  case 'w':  case 'x':  case 'y':  case 'z':
    case 'A':  case 'B':  case 'C':  case 'D':  case 'E':  case 'F':  case 'G':
    case 'H':  case 'I':  case 'J':  case 'K':  case 'L':  case 'M':  case 'N':
    case 'O':  case 'P':  case 'Q':  case 'R':  case 'S':  case 'T':  case 'U':
    case 'V':  case 'W':  case 'X':  case 'Y':  case 'Z':
    case '_':
    {
      do {
        lexer->stream++;
        lexer->currentPosition++;
      } while (isalnum(*lexer->stream) || *lexer->stream == '_');
      token.name = strinternRange(token.start, lexer->stream);
      token.kind = isKeyword(token.name) ? TOKEN_KEYWORD : TOKEN_NAME;
    } break;

    case '+':  case '&':  case '|':  case ':':
    {
      lexer->stream++;
      lexer->currentPosition++;
      if (*lexer->stream == *(lexer->stream-1) || *lexer->stream == '=') {
        lexer->stream++;
        lexer->currentPosition++;
      }
      token.kind = TOKEN_OPERATOR;
      token.optype = strinternRange(token.start, lexer->stream);
    } break;

    case '-':
    {
      lexer->stream++;
      lexer->currentPosition++;
      if (*lexer->stream == *(lexer->stream-1) || *lexer->stream == '=' || *lexer->stream == '>') {
        lexer->stream++;
        lexer->currentPosition++;
      }
      token.kind = TOKEN_OPERATOR;
      token.optype = strinternRange(token.start, lexer->stream);
    } break;

    case '*':  case '%':  case '^':  case '~':  case '!':  case '=':
    {
      lexer->stream++;
      lexer->currentPosition++;
      if (*lexer->stream == '=') {
        lexer->stream++;
        lexer->currentPosition++;
      }
      token.kind = TOKEN_OPERATOR;
      token.optype = strinternRange(token.start, lexer->stream);
    } break;

    case '/':
    {
      lexer->stream++;
      lexer->currentPosition++;
      if (*lexer->stream == '/') {
        token.kind = TOKEN_COMMENT;
        do {
          lexer->stream++;
          lexer->currentPosition++;
        } while (*lexer->stream != '\n' && *lexer->stream != '\r' && *lexer->stream != '\0');
        token.message = (char*) malloc(lexer->stream - token.start + 1);
        strncpy(token.message, token.start, lexer->stream - token.start);
        token.message[lexer->stream - token.start] = '\0';
      } else if (*lexer->stream == '*') {
        token.kind = TOKEN_COMMENT;
        lexer->stream++;
        lexer->currentPosition++;
        while (*lexer->stream != '*' || *(lexer->stream+1) != '/') {
          if (*lexer->stream == '\0') {
            token.kind = TOKEN_ERROR;
            break;
          }
          lexer->stream++;
          lexer->currentPosition++;
        }
        if (token.kind == TOKEN_ERROR) {
          token.message = (char*) malloc(strlen(token.file) + 4 + 2*sizeof(size_t)*8 + 34 + 1);
          sprintf(token.message, "%s:%d:%d  multi-line comment is never closed",
                  token.file, token.line, token.pos);
        } else {
          lexer->stream += 2;
          lexer->currentPosition += 2;
          token.message = (char*) malloc(lexer->stream - token.start + 1);
          strncpy(token.message, token.start, lexer->stream - token.start);
          token.message[lexer->stream - token.start] = '\0';
        }
      } else {
        if (*lexer->stream == '=') {
          lexer->stream++;
          lexer->currentPosition++;
        }
        token.kind = TOKEN_OPERATOR;
        token.optype = strinternRange(token.start, lexer->stream);
      }
    } break;

    case '<':  case '>':
    {
      lexer->stream++;
      lexer->currentPosition++;
      if (*lexer->stream == *(lexer->stream-1) || *lexer->stream == '=') {
        lexer->stream++;
        lexer->currentPosition++;
        if (*lexer->stream == *(lexer->stream-2) || *lexer->stream == '=') {
          lexer->stream++;
          lexer->currentPosition++;
          if (*lexer->stream == '=') {
            lexer->stream++;
            lexer->currentPosition++;
          }
        }
      }
      token.kind = TOKEN_OPERATOR;
      token.optype = strinternRange(token.start, lexer->stream);
    } break;

    case '.':
    {
      lexer->stream++;
      lexer->currentPosition++;
      if (*lexer->stream == *(lexer->stream-1)) {
        lexer->stream++;
        lexer->currentPosition++;
      }
      token.kind = TOKEN_OPERATOR;
      token.optype = strinternRange(token.start, lexer->stream);
    } break;

    case ',':  case ';':  case '(':  case ')':  case '[':  case ']':  case '{':  case '}':
    {
      lexer->stream++;
      lexer->currentPosition++;
      token.kind = TOKEN_SEPARATOR;
      token.optype = strinternRange(token.start, lexer->stream);
    } break;

    default:
    {
      lexer->stream++;
      lexer->currentPosition++;
      token.kind = TOKEN_ERROR;
      token.message = (char*) malloc(strlen(token.file) + 4 + 2*sizeof(size_t)*8 + 18 + 11 + 1);
      sprintf(token.message, "%s:%d:%d  illegal character ", token.file, token.line, token.pos);
      if (isprint(*token.start)) {
        sprintf(token.message + strlen(token.message), "'%c'", *token.start);
      } else {
        sprintf(token.message + strlen(token.message), "'\\?'(0x%02X)", *token.start);
      }
    } break;
  }
  token.end = lexer->stream;

  return token;
}
