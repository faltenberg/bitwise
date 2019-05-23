#include "lexer.h"

#include <ctype.h>


static TokenLoc loc(int line, int pos) {
  return (TokenLoc){ .line=line, .pos=pos };
}


Lexer lexerFromSource(Source src) {
  return (Lexer){ .source=src, .index=0, .currentLoc=loc(0, 0), .nextLoc=loc(1, 1) };
}


static char nextChar(Lexer* lexer) {
  char c = lexer->source.content.chars[lexer->index];
  if (lexer->index < lexer->source.content.len) {
    lexer->index++;
  }

  lexer->currentLoc = lexer->nextLoc;
  if (c == '\n') {
    lexer->nextLoc.line++;
    lexer->nextLoc.pos = 1;
  } else if (c != '\0') {
    lexer->nextLoc.pos++;
  }

  return c;
}


static char peekChar(Lexer* lexer) {
  if (lexer->index < lexer->source.content.len) {
    return lexer->source.content.chars[lexer->index];
  } else {
    return '\0';
  }
}


Token nextToken(Lexer* lexer) {
  Token token = (Token){ .kind=TOKEN_NONE, .source=&lexer->source,
                         .start=loc(0, 0), .end=loc(0, 0), .chars=stringFromArray("") };

  char c = peekChar(lexer);
  while (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
    nextChar(lexer);
    c = peekChar(lexer);
  }

  const char* start = &lexer->source.content.chars[lexer->index];
  char currentChar = nextChar(lexer);
  token.start = lexer->currentLoc;
  switch (currentChar) {
    case '\0':
    {
      token.kind = TOKEN_EOF;
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
      token.kind = TOKEN_NAME;
      while (isalnum(peekChar(lexer)) || peekChar(lexer) == '_') {
        currentChar = nextChar(lexer);
      }
      string name = stringFromRange(start, &lexer->source.content.chars[lexer->index]);
      if (isKeyword(name)) {
        token.kind = TOKEN_KEYWORD;
      }
    } break;


    case '0':
    {
      token.kind = TOKEN_INT;
      if (peekChar(lexer) == 'x' || peekChar(lexer) == 'X') {
        token.kind = TOKEN_ERROR;  // assume error if loop is not entered (thus no digits)
        currentChar = nextChar(lexer);
        bool hasDigit = false;
        while (isdigit(peekChar(lexer)) || peekChar(lexer) == '_' ||
               (peekChar(lexer) >= 'a' && peekChar(lexer) <= 'f') ||
               (peekChar(lexer) >= 'A' && peekChar(lexer) <= 'F')) {
          token.kind = TOKEN_INT;
          currentChar = nextChar(lexer);
          hasDigit |= (currentChar != '_');
        }
        if (!hasDigit || isalpha(peekChar(lexer))) {
          token.kind = TOKEN_ERROR;
          token.chars = stringFromArray("invalid hex integer");
          while (isalnum(peekChar(lexer)) || peekChar(lexer) == '_') {
            currentChar = nextChar(lexer);
          }
        }
        break;
      }
      if (peekChar(lexer) == 'b' || peekChar(lexer) == 'B') {
        token.kind = TOKEN_ERROR;  // assume error if loop is not entered (thus no digits)
        currentChar = nextChar(lexer);
        while (peekChar(lexer) == '0' || peekChar(lexer) == '1') {
          token.kind = TOKEN_INT;
          currentChar = nextChar(lexer);
        }
        if (isalnum(peekChar(lexer))) {
          token.kind = TOKEN_ERROR;
          token.chars = stringFromArray("invalid bin integer");
          while (isalnum(peekChar(lexer))) {
            currentChar = nextChar(lexer);
          }
        }
        break;
      }
    }

    case '1':  case '2':  case '3':  case '4':
    case '5':  case '6':  case '7':  case '8':  case '9':
    {
      token.kind = TOKEN_INT;
      while (isdigit(peekChar(lexer)) || peekChar(lexer) == '_') {
        currentChar = nextChar(lexer);
      }
      if (isalpha(peekChar(lexer))) {
        token.kind = TOKEN_ERROR;
        token.chars = stringFromArray("invalid integer");
        while (isalnum(peekChar(lexer))) {
          currentChar = nextChar(lexer);
        }
      }
    } break;

    default:
    {
      // TODO: create proper error message
      // NOTE: make sure not to overwrite the message later before return!
      token.kind = TOKEN_ERROR;
      token.chars = stringFromArray("could not parse character '$$$' to token");
    } break;
  }
  token.end = lexer->currentLoc;
  const char* end = &lexer->source.content.chars[lexer->index];
  token.chars = stringFromRange(start, (token.kind == TOKEN_EOF) ? end-1 : end);

  return token;
}
