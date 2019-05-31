#include "lexer.h"

#include "str.h"
#include "error.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>


#define RED "\e[31m"
#define GRN "\e[32m"
#define BLU "\e[34m"
#define RST "\e[39m"


Lexer lexerFromSource(const Source* src) {
  return (Lexer){ .source=src, .index=0, .currentChar='\0',
                  .currentLoc=loc(0, 0), .nextLoc=loc(1, 1)
                };
}


static bool isKeyword(string s) {
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


static char nextChar(Lexer* lexer) {
  lexer->currentChar = lexer->source->content.chars[lexer->index];
  if (lexer->index < lexer->source->content.len) {
    lexer->index++;
  }

  lexer->currentLoc = lexer->nextLoc;
  if (lexer->currentChar == '\n') {
    lexer->nextLoc.line++;
    lexer->nextLoc.pos = 1;
  } else if (lexer->currentChar != '\0') {
    lexer->nextLoc.pos++;
  }

  return lexer->currentChar;
}


static char peekChar(Lexer* lexer) {
  if (lexer->index < lexer->source->content.len) {
    return lexer->source->content.chars[lexer->index];
  } else {
    return '\0';
  }
}


Token nextToken(Lexer* lexer) {
  Token token = (Token){ .kind=TOKEN_NONE, .source=lexer->source,
                         .start=loc(0, 0), .end=loc(0, 0), .chars=stringFromArray("") };
  for (char c = peekChar(lexer);
       c == ' ' || c == '\t' || c == '\r' || c == '\n';
       c = peekChar(lexer)) {
    nextChar(lexer);
  }

  const char* start = &lexer->source->content.chars[lexer->index];
  char c = nextChar(lexer);
  token.start = lexer->currentLoc;
  Location errorLoc = loc(0, 0);
  string errorMsg = stringFromArray("");

  switch (c) {
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
      for (char c = peekChar(lexer); isalnum(c) || c == '_'; c = peekChar(lexer)) {
        nextChar(lexer);
      }
      string name = stringFromRange(start, &lexer->source->content.chars[lexer->index]);
      if (isKeyword(name)) {
        token.kind = TOKEN_KEYWORD;
      }
    } break;

    // integer 0, hex and bin integers
    case '0':
    {
      token.kind = TOKEN_INT;
      if (peekChar(lexer) == 'x' || peekChar(lexer) == 'X') {
        token.kind = TOKEN_ERROR;  // assume error if loop is not entered (thus no digits)
        nextChar(lexer);
        bool hasDigit = false;
        for (char c = peekChar(lexer);
             isdigit(c) || c == '_' || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
             c = peekChar(lexer)) {
          token.kind = TOKEN_INT;
          hasDigit |= (nextChar(lexer) != '_');
        }
        if (!hasDigit) {
          token.kind = TOKEN_ERROR;
          errorLoc = lexer->currentLoc;
          errorMsg = stringFromArray("hex integer must have at least one digit");
        } else if (isalpha(peekChar(lexer))) {
          nextChar(lexer);
          token.kind = TOKEN_ERROR;
          errorLoc = lexer->currentLoc;
          errorMsg = stringFromArray("invalid hex integer format");
          for (char c = peekChar(lexer); isalnum(c) || c == '_'; c = peekChar(lexer)) {
            nextChar(lexer);
          }
        }
        break;
      }
      if (peekChar(lexer) == 'b' || peekChar(lexer) == 'B') {
        token.kind = TOKEN_ERROR;  // assume error if loop is not entered (thus no digits)
        nextChar(lexer);
        bool hasDigit = false;
        for (char c = peekChar(lexer); c == '0' || c == '1' || c == '_'; c = peekChar(lexer)) {
          token.kind = TOKEN_INT;
          hasDigit |= (nextChar(lexer) != '_');
        }
        if (!hasDigit) {
          token.kind = TOKEN_ERROR;
          errorLoc = lexer->currentLoc;
          errorMsg = stringFromArray("bin integer must have at least one digit");
        } else if (isalnum(peekChar(lexer))) {
          nextChar(lexer);
          token.kind = TOKEN_ERROR;
          errorLoc = lexer->currentLoc;
          errorMsg = stringFromArray("invalid bin integer format");
          for (char c = peekChar(lexer); isalnum(c) || c == '_'; c = peekChar(lexer)) {
            nextChar(lexer);
          }
        }
        break;
      }
    }  // shall fall trough

    case '1':  case '2':  case '3':  case '4':
    case '5':  case '6':  case '7':  case '8':  case '9':
    {
      token.kind = TOKEN_INT;
      for (char c = peekChar(lexer); isdigit(c) || c == '_'; c = peekChar(lexer)) {
        nextChar(lexer);
      }
      if (isalpha(peekChar(lexer))) {
        nextChar(lexer);
        token.kind = TOKEN_ERROR;
        errorLoc = lexer->currentLoc;
        errorMsg = stringFromArray("invalid integer format");
        for (char c = peekChar(lexer); isalnum(c); c = peekChar(lexer)) {
          nextChar(lexer);
        }
      }
    } break;

    // separators "(" ")" "[" "]" "{" "}" "," ";" ":" "."
    case '(':  case ')':  case '[':  case ']':  case '{':  case '}':
    case ',':  case ';':  case ':':  case '.':
    {
      token.kind = TOKEN_SYMBOL;
    } break;

    // operators "!" "!=" "=" "==" "<" "<=" ">" ">="
    case '!':  case '=':  case '<':  case '>':
    {
      token.kind = TOKEN_SYMBOL;
      if (peekChar(lexer) == '=') {
        nextChar(lexer);
      }
    } break;

    // operators "+" "++" "&" "&&" "|" "||"
    case '+':  case '&':  case '|':
    {
      token.kind = TOKEN_SYMBOL;
      if (peekChar(lexer) == lexer->currentChar) {
        nextChar(lexer);
      }
    } break;

    // operators "*" "%" "^" "~"
    case '*':  case '%':  case '^':  case '~':
    {
      token.kind = TOKEN_SYMBOL;
    } break;

    // operators "-" "--" "->"
    case '-':
    {
      token.kind = TOKEN_SYMBOL;
      if (peekChar(lexer) == '>' || peekChar(lexer) == lexer->currentChar) {
        nextChar(lexer);
      }
    } break;

    // operator "/" and single-line and multi-line comments
    case '/':
    {
      token.kind = TOKEN_SYMBOL;

      if (peekChar(lexer) == '/') {  // munch single-line comment
        token.kind = TOKEN_COMMENT;
        for (char c = peekChar(lexer); c != '\0' && c != '\n'; c = peekChar(lexer)) {
          nextChar(lexer);
        }
      } else if (peekChar(lexer) == '*') {  // munch multi-line comment
        nextChar(lexer);
        for (char c = peekChar(lexer); c != '\0'; c = peekChar(lexer)) {
          if (peekChar(lexer) == '*') {
            nextChar(lexer);
            if (peekChar(lexer) == '/') {
              token.kind = TOKEN_COMMENT;
              nextChar(lexer);
              break;
            }
          } else {
            nextChar(lexer);
          }
        }
        if (token.kind != TOKEN_COMMENT) {
          token.kind = TOKEN_ERROR;
          errorLoc = token.start;
          errorMsg = stringFromArray("unclosed multi-line comment");
        }
      }
    } break;

    default:
    {
      token.kind = TOKEN_ERROR;
      errorLoc = lexer->currentLoc;
      errorMsg = stringFromArray("illegal character");
    } break;
  }

  token.end = lexer->currentLoc;
  const char* end = &lexer->source->content.chars[lexer->index];
  token.chars = stringFromRange(start, (token.kind == TOKEN_EOF) ? end-1 : end);
  if (token.kind == TOKEN_ERROR) {
    token.error = (Error*) malloc(sizeof(Error));
    *token.error = generateError(errorLoc, token.source, token.start, token.end,
                                 errorMsg.chars);
  }

  return token;
}

