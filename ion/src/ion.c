#include "sbuffer.h"
#include "lexer.h"

#include <stdbool.h>
#include <stdio.h>
#include <assert.h>


#define NOP ' '
#define LIT '#'
#define ADD '+'
#define SUB '-'
#define NEG '~'
#define MUL '*'
#define DIV '/'
#define HLT '\0'


static Lexer lexer;
static Token currentToken;
static unsigned char* code;


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


static void parse_expr0();
static void parse_expr3() {
  if (isTokenKind(&currentToken, TOKEN_INT)) {
    bufPush(code, LIT);
    int value = currentToken.value;
    bufPush(code, (unsigned char) value >> 0);
    bufPush(code, (unsigned char) value >> 8);
    bufPush(code, (unsigned char) value >> 16);
    bufPush(code, (unsigned char) value >> 24);
    getNextToken();
  } else if (matchToken('(')) {
    parse_expr0();
    expectToken(')');
  } else {
    printf("ERROR: expected TOKEN_INT or '(' but got Token(%d)\n", currentToken.kind);
    exit(1);
  }
}


static void parse_expr2() {
  if (matchToken('-')) {
    parse_expr3();
    bufPush(code, NEG);
  } else {
    parse_expr3();
  }
}


static void parse_expr1() {
  parse_expr2();
  if (isTokenKind(&currentToken, '*') || isTokenKind(&currentToken, '/')) {
    char op = currentToken.kind;
    getNextToken();
    parse_expr2();
    bufPush(code, op);
  }
}


static void parse_expr0() {
  parse_expr1();
  if (isTokenKind(&currentToken, '+') || isTokenKind(&currentToken, '-')) {
    char op = currentToken.kind;
    getNextToken();
    parse_expr1();
    bufPush(code, op);
  }
}


/**
 * expr3 = INT | '(' expr ')'
 * expr2 = [-]? expr3
 * expr1 = expr2 ([/*] expr2)*
 * expr0 = expr1 ([+-] expr1)*
 * expr = expr0
 */
static void parse_expr() {
  parse_expr0();
  bufPush(code, HLT);
}


#define MAX_STACK  1024
#define POP()      (*top--)
#define PUSH(x)    (*++top = (x))
#define POPS(n)    assert(top - stack >= (n))
#define PUSHES(n)  assert(top + (n) <= stack + MAX_STACK)

int vm_exec(const unsigned char* code) {
  int stack[MAX_STACK];
  int* top = stack;
  *top = 0xDEADBEEF;

  for (int i = 0, running = 1; running; i++) {
    switch (code[i]) {
      case NOP:
        continue;

      case LIT: {
        int value = (code[++i] << 0) + (code[++i] << 8) + (code[++i] << 16) + (code[++i] << 24);
        PUSHES(1);
        PUSH(value);
      } break;

      case ADD: {
        POPS(2);
        int r = POP();
        int l = POP();
        PUSHES(1);
        PUSH(l + r);
      } break;

      case SUB: {
        POPS(2);
        int r = POP();
        int l = POP();
        PUSHES(1);
        PUSH(l - r);
      } break;

      case NEG: {
        POPS(1);
        int v = POP();
        PUSHES(1);
        PUSH(-v);
      } break;

      case MUL: {
        POPS(2);
        int r = POP();
        int l = POP();
        PUSHES(1);
        PUSH(l * r);
      } break;

      case DIV: {
        POPS(2);
        int r = POP();
        int l = POP();
        PUSHES(1);
        PUSH(l / r);
      } break;

      case HLT:
        running = 0;
        break;

      default:
        printf("FATAL: unknown opcode(%c)\n", code[i]);
    }
  }

  return *top;
}


static void test_parse_expr(const char* expr, int expected) {
  printf("Parse: \"%s\"\n", expr);
  lexer = newLexer(expr);
  currentToken = (Token) {};
  getNextToken();
  code = NULL;
  parse_expr();
  printf("  Exec {");
  for (int i = 0; code[i] != HLT; i++) {
    printf(" %c", code[i]);
    if (code[i] == LIT) {
      int value = (code[++i] << 0) + (code[++i] << 8) + (code[++i] << 16) + (code[++i] << 24);
      printf("%d", value);
      continue;
    }
  }
  printf(" } ...\n");
  int value = vm_exec(code);
  if (value != expected) {
    printf("ERROR: expected %d == %d\n", value, expected);
  }
  bufFree(code);
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
  TEST_EXPR(1 * (2 + 3));
  TEST_EXPR((1 + 2) / (2 - 3));
  return 0;
}
