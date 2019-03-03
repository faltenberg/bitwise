#include "cunit.h"

#include "lexer.h"
#include "sbuffer.h"
#include "strintern.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>


typedef struct _Test {
  char*        name;
  const char*  input;
  Token*       expected;
} _Test;


static SBUF(_Test) _tests       = NULL;
static size_t      _currentTest = 0;


static void printToken(const Token* token) {
  switch (token->kind) {
    case TOKEN_EOF:
      printVerbose("Token[ TOKEN_EOF ]");
      break;
    case TOKEN_INT:
      printVerbose("Token[ TOKEN_INT: %lu ]", token->value);
      break;
    case TOKEN_NAME:
      printVerbose("Token[ TOKEN_NAME: %s ]", token->name);
      break;
    case TOKEN_KEYWORD:
      printVerbose("Token[ TOKEN_KEYWORD: %s ]", token->name);
      break;
    case TOKEN_OPERATOR:
      printVerbose("Token[ TOKEN_OPERATOR: %s ]", token->optype);
      break;
    case TOKEN_SEPARATOR:
      printVerbose("Token[ TOKEN_SEPARATOR: %s ]", token->optype);
      break;
    case TOKEN_COMMENT:
      printVerbose("Token[ TOKEN_COMMENT: \"%s\" ]", token->optype);
      break;
    case TOKEN_ERROR:
      printVerbose("Token[ TOKEN_ERROR: %s ]", token->message);
      break;
    default:
      printVerbose("Token[ UNKNOWN_TYPE: '%c'(0x%02X) ]", token->kind, token->kind);
  }
}


#define assertEqualToken(idx, tok, exp) __assertEqualToken(__FILE__, __LINE__, idx, tok, exp)
static bool __assertEqualToken(const char* file, int line,
                               int index, const Token* token, const Token* expected) {
  printVerbose(__PROMPT, file, line);

  if (token->kind != expected->kind ||
      (expected->kind == TOKEN_INT       && token->value  != expected->value)  ||
      (expected->kind == TOKEN_NAME      && token->name   != expected->name)   ||
      (expected->kind == TOKEN_KEYWORD   && token->name   != expected->name)   ||
      (expected->kind == TOKEN_OPERATOR  && token->optype != expected->optype) ||
      (expected->kind == TOKEN_SEPARATOR && token->optype != expected->optype) ) {
    printVerbose(RED "ERROR: " RST);
    printVerbose("in tokens[%d] expected ", index);
    printToken(token);
    printVerbose(" == ");
    printToken(expected);
    printVerbose("\n");
    return false;
  } else {
    printVerbose(GRN "OK\n" RST);
    return true;
  }
}


static TestResult test() {
  TestResult result = {};
  Lexer lexer = newLexer("src", _tests[_currentTest].input);
  Token* expected = _tests[_currentTest].expected;

  for (size_t i = 0; i < bufLength(expected); i++) {
    Token token = nextToken(&lexer);
    TEST(assertEqualToken(i, &token, &expected[i]));
  }

  _currentTest++;
  return result;
}


static void createTest(TestSuite* suite, const char* input, size_t numTokens, ...) {
  char* testName = (char*) malloc(8 + strlen(input) + 1);
  sprintf(testName, "parse \"%s\"", input);

  _Test t = (_Test){ .name=testName, .input=input, .expected=NULL };

  va_list args;
  va_start(args, numTokens);
  for (size_t i = 0; i < numTokens; i++) {
    Token token = va_arg(args, Token);
    bufPush(t.expected, token);
  }
  va_end(args);

  bufPush(_tests, t);

  addTest(suite, &test, testName);
}


static void deleteTests() {
  for (size_t i = 0; i < bufLength(_tests); i++) {
    free(_tests[i].name);
    bufFree(_tests[i].expected);
  }
  bufFree(_tests);
  _tests = NULL;
  _currentTest = 0;
}


static Token tokenVarg(int line, int pos, TokenKind kind, va_list args) {
  Token token = { .file="exp", .line=line, .pos=pos, .kind=kind, .start="", .end="" };
  if (kind == TOKEN_INT) {
    token.value = va_arg(args, uint64_t);
  } else if (kind == TOKEN_NAME || kind == TOKEN_KEYWORD) {
    token.name = strintern(va_arg(args, const char*));
  } else if (kind == TOKEN_OPERATOR || kind == TOKEN_SEPARATOR) {
    token.optype = strintern(va_arg(args, const char*));
  } else if (kind == TOKEN_ERROR) {
    token.message = "error message";
  }
  return token;
}


static Token token(TokenKind kind, ...) {
  va_list args;
  va_start(args, kind);
  Token token = tokenVarg(1, 1, kind, args);
  va_end(args);
  return token;
}


static Token tokenLinePos(int line, int pos, TokenKind kind, ...) {
  va_list args;
  va_start(args, kind);
  Token token = tokenVarg(line, pos, kind, args);
  va_end(args);
  return token;
}


static TestResult testLineCounting_test() {
  TestResult result = {};
  Lexer lexer = newLexer("src", _tests[_currentTest].input);
  Token* expected = _tests[_currentTest].expected;

  for (size_t i = 0; i < bufLength(expected); i++) {
    Token token = nextToken(&lexer);
    TEST(assertEqualToken(i, &token, &expected[i]));
    TEST(assertEqualInt(token.line, expected[i].line));
    TEST(assertEqualInt(token.pos, expected[i].pos));
  }

  _currentTest++;
  return result;
}


static void testLineCounting(TestSuite* suite) {
  const char* input = "a + \n b = \n\n c;";
  char* testName = (char*) malloc(8 + strlen(input) + 1);
  sprintf(testName, "parse \"%s\"", input);

  _Test t = (_Test){ .name=testName, .input=input, .expected=NULL };
  bufPush(t.expected, tokenLinePos(1, 1, TOKEN_NAME, "a"));
  bufPush(t.expected, tokenLinePos(1, 3, TOKEN_OPERATOR, "+"));
  bufPush(t.expected, tokenLinePos(2, 2, TOKEN_NAME, "b"));
  bufPush(t.expected, tokenLinePos(2, 4, TOKEN_OPERATOR, "="));
  bufPush(t.expected, tokenLinePos(4, 2, TOKEN_NAME, "c"));
  bufPush(t.expected, tokenLinePos(4, 3, TOKEN_SEPARATOR, ";"));
  bufPush(t.expected, tokenLinePos(4, 4, TOKEN_EOF));
  bufPush(_tests, t);

  addTest(suite, &testLineCounting_test, testName);
}


TestResult lexer_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<lexer>", "Test lexer for ION grammar.");
  const char* in;

  in = "";
  createTest(&suite, in, 1,
             token(TOKEN_EOF)
            );

  in = " ";
  createTest(&suite, in, 1,
             token(TOKEN_EOF)
            );

  in = "\t";
  createTest(&suite, in, 1,
             token(TOKEN_EOF)
            );

  in = "\n";
  createTest(&suite, in, 1,
             token(TOKEN_EOF)
            );

  in = "1";
  createTest(&suite, in, 2,
             token(TOKEN_INT, 1),
             token(TOKEN_EOF)
            );

  in = "234";
  createTest(&suite, in, 2,
             token(TOKEN_INT, 234),
             token(TOKEN_EOF)
            );

  in = "0567";
  createTest(&suite, in, 2,
             token(TOKEN_INT, 567),
             token(TOKEN_EOF)
            );

  in = " 8";
  createTest(&suite, in, 2,
             token(TOKEN_INT, 8),
             token(TOKEN_EOF)
            );

  in = "9 ";
  createTest(&suite, in, 2,
             token(TOKEN_INT, 9),
             token(TOKEN_EOF)
            );

  in = "a";
  createTest(&suite, in, 2,
             token(TOKEN_NAME, "a"),
             token(TOKEN_EOF)
            );

  in = "abcdefghijklmnopqrstuvwxyz";
  createTest(&suite, in, 2,
             token(TOKEN_NAME, "abcdefghijklmnopqrstuvwxyz"),
             token(TOKEN_EOF)
            );

  in = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  createTest(&suite, in, 2,
             token(TOKEN_NAME, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"),
             token(TOKEN_EOF)
            );

  in = "_";
  createTest(&suite, in, 2,
             token(TOKEN_NAME, "_"),
             token(TOKEN_EOF)
            );

  in = "_a1";
  createTest(&suite, in, 2,
             token(TOKEN_NAME, "_a1"),
             token(TOKEN_EOF)
            );

  in = "a1_";
  createTest(&suite, in, 2,
             token(TOKEN_NAME, "a1_"),
             token(TOKEN_EOF)
            );

  in = "a_1";
  createTest(&suite, in, 2,
             token(TOKEN_NAME, "a_1"),
             token(TOKEN_EOF)
            );

  in = "1a";
  createTest(&suite, in, 2,
             token(TOKEN_ERROR),
             token(TOKEN_EOF)
            );

  in = "1_a";
  createTest(&suite, in, 2,
             token(TOKEN_ERROR),
             token(TOKEN_EOF)
            );

  in = "a 1";
  createTest(&suite, in, 3,
             token(TOKEN_NAME, "a"),
             token(TOKEN_INT, 1),
             token(TOKEN_EOF)
            );

  in = "\\ $ # @ ` \" \' ?";
  createTest(&suite, in, 9,
             token(TOKEN_ERROR),
             token(TOKEN_ERROR),
             token(TOKEN_ERROR),
             token(TOKEN_ERROR),
             token(TOKEN_ERROR),
             token(TOKEN_ERROR),
             token(TOKEN_ERROR),
             token(TOKEN_ERROR),
             token(TOKEN_EOF)
            );

  in = "+ ++ +=";
  createTest(&suite, in, 4,
             token(TOKEN_OPERATOR, "+"),
             token(TOKEN_OPERATOR, "++"),
             token(TOKEN_OPERATOR, "+="),
             token(TOKEN_EOF)
            );

  in = "- -- -= ->";
  createTest(&suite, in, 5,
             token(TOKEN_OPERATOR, "-"),
             token(TOKEN_OPERATOR, "--"),
             token(TOKEN_OPERATOR, "-="),
             token(TOKEN_OPERATOR, "->"),
             token(TOKEN_EOF)
            );

  in = "* *=";
  createTest(&suite, in, 3,
             token(TOKEN_OPERATOR, "*"),
             token(TOKEN_OPERATOR, "*="),
             token(TOKEN_EOF)
            );

  in = "/ /=";
  createTest(&suite, in, 3,
             token(TOKEN_OPERATOR, "/"),
             token(TOKEN_OPERATOR, "/="),
             token(TOKEN_EOF)
            );

  in = "% %=";
  createTest(&suite, in, 3,
             token(TOKEN_OPERATOR, "%"),
             token(TOKEN_OPERATOR, "%="),
             token(TOKEN_EOF)
            );

  in = "& && &=";
  createTest(&suite, in, 4,
             token(TOKEN_OPERATOR, "&"),
             token(TOKEN_OPERATOR, "&&"),
             token(TOKEN_OPERATOR, "&="),
             token(TOKEN_EOF)
            );

  in = "| || |=";
  createTest(&suite, in, 4,
             token(TOKEN_OPERATOR, "|"),
             token(TOKEN_OPERATOR, "||"),
             token(TOKEN_OPERATOR, "|="),
             token(TOKEN_EOF)
            );

  in = "^ ^=";
  createTest(&suite, in, 3,
             token(TOKEN_OPERATOR, "^"),
             token(TOKEN_OPERATOR, "^="),
             token(TOKEN_EOF)
            );

  in = "~ ~=";
  createTest(&suite, in, 3,
             token(TOKEN_OPERATOR, "~"),
             token(TOKEN_OPERATOR, "~="),
             token(TOKEN_EOF)
            );

  in = "< <= << <<= <<< <<<=";
  createTest(&suite, in, 7,
             token(TOKEN_OPERATOR, "<"),
             token(TOKEN_OPERATOR, "<="),
             token(TOKEN_OPERATOR, "<<"),
             token(TOKEN_OPERATOR, "<<="),
             token(TOKEN_OPERATOR, "<<<"),
             token(TOKEN_OPERATOR, "<<<="),
             token(TOKEN_EOF)
            );

  in = "> >= >> >>= >>> >>>=";
  createTest(&suite, in, 7,
             token(TOKEN_OPERATOR, ">"),
             token(TOKEN_OPERATOR, ">="),
             token(TOKEN_OPERATOR, ">>"),
             token(TOKEN_OPERATOR, ">>="),
             token(TOKEN_OPERATOR, ">>>"),
             token(TOKEN_OPERATOR, ">>>="),
             token(TOKEN_EOF)
            );

  in = "! !=";
  createTest(&suite, in, 3,
             token(TOKEN_OPERATOR, "!"),
             token(TOKEN_OPERATOR, "!="),
             token(TOKEN_EOF)
            );

  in = "= ==";
  createTest(&suite, in, 3,
             token(TOKEN_OPERATOR, "="),
             token(TOKEN_OPERATOR, "=="),
             token(TOKEN_EOF)
            );

  in = ". ..";
  createTest(&suite, in, 3,
             token(TOKEN_OPERATOR, "."),
             token(TOKEN_OPERATOR, ".."),
             token(TOKEN_EOF)
            );

  in = ": :: :=";
  createTest(&suite, in, 4,
             token(TOKEN_OPERATOR, ":"),
             token(TOKEN_OPERATOR, "::"),
             token(TOKEN_OPERATOR, ":="),
             token(TOKEN_EOF)
            );

  in = ", ; ( ) [ ] { }";
  createTest(&suite, in, 9,
             token(TOKEN_SEPARATOR, ","),
             token(TOKEN_SEPARATOR, ";"),
             token(TOKEN_SEPARATOR, "("),
             token(TOKEN_SEPARATOR, ")"),
             token(TOKEN_SEPARATOR, "["),
             token(TOKEN_SEPARATOR, "]"),
             token(TOKEN_SEPARATOR, "{"),
             token(TOKEN_SEPARATOR, "}"),
             token(TOKEN_EOF)
            );

  in = "if else do while for switch case";
  createTest(&suite, in, 8,
             token(TOKEN_KEYWORD, "if"),
             token(TOKEN_KEYWORD, "else"),
             token(TOKEN_KEYWORD, "do"),
             token(TOKEN_KEYWORD, "while"),
             token(TOKEN_KEYWORD, "for"),
             token(TOKEN_KEYWORD, "switch"),
             token(TOKEN_KEYWORD, "case"),
             token(TOKEN_EOF)
            );

  in = "continue break return true false bool";
  createTest(&suite, in, 7,
             token(TOKEN_KEYWORD, "continue"),
             token(TOKEN_KEYWORD, "break"),
             token(TOKEN_KEYWORD, "return"),
             token(TOKEN_KEYWORD, "true"),
             token(TOKEN_KEYWORD, "false"),
             token(TOKEN_KEYWORD, "bool"),
             token(TOKEN_EOF)
            );

  in = "int var const func struct";
  createTest(&suite, in, 6,
             token(TOKEN_KEYWORD, "int"),
             token(TOKEN_KEYWORD, "var"),
             token(TOKEN_KEYWORD, "const"),
             token(TOKEN_KEYWORD, "func"),
             token(TOKEN_KEYWORD, "struct"),
             token(TOKEN_EOF)
            );

  in = "a + b";
  createTest(&suite, in, 4,
             token(TOKEN_NAME, "a"),
             token(TOKEN_OPERATOR, "+"),
             token(TOKEN_NAME, "b"),
             token(TOKEN_EOF)
            );

  in = "(1)";
  createTest(&suite, in, 4,
             token(TOKEN_SEPARATOR, "("),
             token(TOKEN_INT, 1),
             token(TOKEN_SEPARATOR, ")"),
             token(TOKEN_EOF)
            );

  in = "a[1]";
  createTest(&suite, in, 5,
             token(TOKEN_NAME, "a"),
             token(TOKEN_SEPARATOR, "["),
             token(TOKEN_INT, 1),
             token(TOKEN_SEPARATOR, "]"),
             token(TOKEN_EOF)
            );

  in = "((a+b)/2)";
  createTest(&suite, in, 10,
             token(TOKEN_SEPARATOR, "("),
             token(TOKEN_SEPARATOR, "("),
             token(TOKEN_NAME, "a"),
             token(TOKEN_OPERATOR, "+"),
             token(TOKEN_NAME, "b"),
             token(TOKEN_SEPARATOR, ")"),
             token(TOKEN_OPERATOR, "/"),
             token(TOKEN_INT, 2),
             token(TOKEN_SEPARATOR, ")"),
             token(TOKEN_EOF)
            );

  in = "(2*(a-b))";
  createTest(&suite, in, 10,
             token(TOKEN_SEPARATOR, "("),
             token(TOKEN_INT, 2),
             token(TOKEN_OPERATOR, "*"),
             token(TOKEN_SEPARATOR, "("),
             token(TOKEN_NAME, "a"),
             token(TOKEN_OPERATOR, "-"),
             token(TOKEN_NAME, "b"),
             token(TOKEN_SEPARATOR, ")"),
             token(TOKEN_SEPARATOR, ")"),
             token(TOKEN_EOF)
            );

  in = "a + -2";
  createTest(&suite, in, 5,
             token(TOKEN_NAME, "a"),
             token(TOKEN_OPERATOR, "+"),
             token(TOKEN_OPERATOR, "-"),
             token(TOKEN_INT, 2),
             token(TOKEN_EOF)
            );

  in = "a + +b";
  createTest(&suite, in, 5,
             token(TOKEN_NAME, "a"),
             token(TOKEN_OPERATOR, "+"),
             token(TOKEN_OPERATOR, "+"),
             token(TOKEN_NAME, "b"),
             token(TOKEN_EOF)
            );

  in = "a - -2";
  createTest(&suite, in, 5,
             token(TOKEN_NAME, "a"),
             token(TOKEN_OPERATOR, "-"),
             token(TOKEN_OPERATOR, "-"),
             token(TOKEN_INT, 2),
             token(TOKEN_EOF)
            );

  in = "a * **2";
  createTest(&suite, in, 6,
             token(TOKEN_NAME, "a"),
             token(TOKEN_OPERATOR, "*"),
             token(TOKEN_OPERATOR, "*"),
             token(TOKEN_OPERATOR, "*"),
             token(TOKEN_INT, 2),
             token(TOKEN_EOF)
            );

  in = "if (a && !b)";
  createTest(&suite, in, 7,
             token(TOKEN_KEYWORD, "if"),
             token(TOKEN_SEPARATOR, "("),
             token(TOKEN_NAME, "a"),
             token(TOKEN_OPERATOR, "&&"),
             token(TOKEN_OPERATOR, "!"),
             token(TOKEN_NAME, "b"),
             token(TOKEN_SEPARATOR, ")"),
             token(TOKEN_EOF)
            );

  in = "else if (x != 0)";
  createTest(&suite, in, 8,
             token(TOKEN_KEYWORD, "else"),
             token(TOKEN_KEYWORD, "if"),
             token(TOKEN_SEPARATOR, "("),
             token(TOKEN_NAME, "x"),
             token(TOKEN_OPERATOR, "!="),
             token(TOKEN_INT, 0),
             token(TOKEN_SEPARATOR, ")"),
             token(TOKEN_EOF)
            );

  in = "for (int x; x <= 2; x++)";
  createTest(&suite, in, 13,
             token(TOKEN_KEYWORD, "for"),
             token(TOKEN_SEPARATOR, "("),
             token(TOKEN_KEYWORD, "int"),
             token(TOKEN_NAME, "x"),
             token(TOKEN_SEPARATOR, ";"),
             token(TOKEN_NAME, "x"),
             token(TOKEN_OPERATOR, "<="),
             token(TOKEN_INT, 2),
             token(TOKEN_SEPARATOR, ";"),
             token(TOKEN_NAME, "x"),
             token(TOKEN_OPERATOR, "++"),
             token(TOKEN_SEPARATOR, ")"),
             token(TOKEN_EOF)
            );

  in = "while (a || b);";
  createTest(&suite, in, 8,
             token(TOKEN_KEYWORD, "while"),
             token(TOKEN_SEPARATOR, "("),
             token(TOKEN_NAME, "a"),
             token(TOKEN_OPERATOR, "||"),
             token(TOKEN_NAME, "b"),
             token(TOKEN_SEPARATOR, ")"),
             token(TOKEN_SEPARATOR, ";"),
             token(TOKEN_EOF)
            );

  in = "object.field = 42;";
  createTest(&suite, in, 7,
             token(TOKEN_NAME, "object"),
             token(TOKEN_OPERATOR, "."),
             token(TOKEN_NAME, "field"),
             token(TOKEN_OPERATOR, "="),
             token(TOKEN_INT, 42),
             token(TOKEN_SEPARATOR, ";"),
             token(TOKEN_EOF)
            );

  in = "statement; // x=3;";
  createTest(&suite, in, 4,
             token(TOKEN_NAME, "statement"),
             token(TOKEN_SEPARATOR, ";"),
             token(TOKEN_COMMENT, "// x=3;"),
             token(TOKEN_EOF)
            );

  in = "//";
  createTest(&suite, in, 2,
             token(TOKEN_COMMENT, "//"),
             token(TOKEN_EOF)
            );

  in = "//\n return";
  createTest(&suite, in, 3,
             token(TOKEN_COMMENT, "//"),
             token(TOKEN_KEYWORD, "return"),
             token(TOKEN_EOF)
            );

  in = "a = /* 3 */;";
  createTest(&suite, in, 5,
             token(TOKEN_NAME, "a"),
             token(TOKEN_OPERATOR, "="),
             token(TOKEN_COMMENT, "/* 3 */"),
             token(TOKEN_SEPARATOR, ";"),
             token(TOKEN_EOF)
            );

  in = "/**/";
  createTest(&suite, in, 2,
             token(TOKEN_COMMENT, "/**/"),
             token(TOKEN_EOF)
            );

  in = "/* some\n comment\n*/";
  createTest(&suite, in, 2,
             token(TOKEN_COMMENT, "/* some\n comment\n*/"),
             token(TOKEN_EOF)
            );

  in = "/*";
  createTest(&suite, in, 2,
             token(TOKEN_ERROR),
             token(TOKEN_EOF)
            );

  in = "/**";
  createTest(&suite, in, 2,
             token(TOKEN_ERROR),
             token(TOKEN_EOF)
            );

  in = "/*/";
  createTest(&suite, in, 2,
             token(TOKEN_ERROR),
             token(TOKEN_EOF)
            );

  in = "/**\n/";
  createTest(&suite, in, 2,
             token(TOKEN_ERROR),
             token(TOKEN_EOF)
            );

  testLineCounting(&suite);

  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  deleteTests();
  strinternFree();
  return result;
}
