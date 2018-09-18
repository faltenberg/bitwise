#include "sbuffer.h"
#include "lexer.h"

#include <stdbool.h>
#include <stdio.h>


static Lexer lexer;
static Token currentToken;


static void printToken(const Token* token) {
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


static void getNextToken() {
  currentToken = nextToken(&lexer);
//  printToken(&currentToken);
}


static bool isTokenKind(const Token* token, TokenKind kind) {
  return token->kind == kind;
}


static bool isTokenName(const Token* token, const char* name) {
  return token->kind == TOKEN_NAME && token->name == name;
}


static bool matchToken(TokenKind kind) {
  if (isTokenKind(&currentToken, kind)) {
    getNextToken();
    return true;
  } else {
    return false;
  }
}


static bool expectToken(TokenKind kind) {
  if (isTokenKind(&currentToken, kind)) {
    getNextToken();
    return true;
  } else {
    printf("ERROR: expected Token(%d), but got Token(%d)\n", kind, currentToken.kind);
    exit(1);
    return false;
  }
}


static int parse_expr();
static int parse_expr3() {
  if (isTokenKind(&currentToken, TOKEN_INT)) {
    int result = currentToken.value;
    getNextToken();
    return result;
  } else if (matchToken('(')) {
    int result = parse_expr();
    expectToken(')');
    return result;
  } else {
    printf("ERROR: expected TOKEN_INT or '(' but got Token(%d)\n", currentToken.kind);
    exit(1);
    return 0;
  }
}


static int parse_expr2() {
  if (matchToken('-')) {
    return -parse_expr3();
  } else {
    return parse_expr3();
  }
}


static int parse_expr1() {
  int a = parse_expr2();
  int result = a;
  if (isTokenKind(&currentToken, '*') || isTokenKind(&currentToken, '/')) {
    char op = currentToken.kind;
    getNextToken();
    int b = parse_expr2();
    result = (op == '*') ? a*b : a/b;
  }
  return result;
}


static int parse_expr0() {
  int a = parse_expr1();
  int result = a;
  if (isTokenKind(&currentToken, '+') || isTokenKind(&currentToken, '-')) {
    char op = currentToken.kind;
    getNextToken();
    int b = parse_expr1();
    result = (op == '+') ? a+b : a-b;
  }
  return result;
}


/**
 * expr3 = INT | '(' expr ')'
 * expr2 = [-]? expr3
 * expr1 = expr2 ([/*] expr2)*
 * expr0 = expr1 ([+-] expr1)*
 * expr = expr0
 */
static int parse_expr() {
  return parse_expr0();
}


static void test_parse_expr(const char* expr, int expected) {
  lexer = newLexer(expr);
  printf("Stream: \"%s\"\n", lexer.stream);
  getNextToken();
  int value = parse_expr();
  if (value != expected) {
    printf("ERROR: expected %d == %d\n", value, expected);
  }
}


#define TEST_EXPR(x) test_parse_expr(#x, (x))

int main(int argc, char** argv) {
  TEST_EXPR(1);
  TEST_EXPR(-1);
  TEST_EXPR((123));
  TEST_EXPR(1 + 2);
  TEST_EXPR((1 - 2));
  TEST_EXPR(1+-2);
  TEST_EXPR(2 * 2);
  TEST_EXPR(4 / 2);
  TEST_EXPR(1 + 2 * 3);
  TEST_EXPR((1 + 2) * 3);
  TEST_EXPR(1 + (2 * 3));
  TEST_EXPR((1 + 2) / (2 - 3));
  return 0;
}
