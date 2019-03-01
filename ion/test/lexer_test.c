#include "cunit/cunit.h"
#include "lexer.h"
#include "sbuffer.h"
#include "strintern.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


static char**       _testNames      = NULL;
static const char** _inputStreams   = NULL;
static Token**      _expectedTokens = NULL;
static size_t       _currentTest    = 0;


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
  size_t index = _currentTest++;
  Lexer lexer = newLexer(_inputStreams[index]);
  Token* tokens = _expectedTokens[index];
  for (size_t i = 0; i < bufLength(tokens); i++) {
    Token token = nextToken(&lexer);
    Token* expected = &tokens[i];
    TEST(assertEqualToken(i, &token, expected));
  }
  return result;
}


static void createTest(TestSuite* suite, const char* input, size_t numTokens, ...) {
  char* testName = (char*) malloc((8 + strlen(input) + 1) * sizeof(char));
  strcpy(testName, "parse \"");
  strcat(testName, input);
  strcat(testName, "\"");
  strcat(testName, "\0");

  bufPush(_testNames, testName);
  bufPush(_inputStreams, input);

  Token* tokens = NULL;
  va_list args;
  va_start(args, numTokens);
  for (size_t i = 0; i < numTokens; i++) {
    Token token = va_arg(args, Token);
    bufPush(tokens, token);
  }
  va_end(args);
  bufPush(_expectedTokens, tokens);

  addTest(suite, &test, testName);
}


static void deleteTests() {
  _currentTest = 0;
  for (size_t i = 0; i < bufLength(_testNames); i++) {
    free(_testNames[i]);
  }
  bufFree(_testNames);
  bufFree(_inputStreams);
  for (size_t i = 0; i < bufLength(_expectedTokens); i++) {
    bufFree(_expectedTokens[i]);
  }
  bufFree(_expectedTokens);
}


static Token token(TokenKind kind, ...) {
  Token token = { .kind=kind };
  if (kind == TOKEN_INT) {
    va_list args;
    va_start(args, kind);
    token.value = va_arg(args, uint64_t);
    va_end(args);
  } else if (kind == TOKEN_NAME || kind == TOKEN_KEYWORD) {
    va_list args;
    va_start(args, kind);
    token.name = strintern(va_arg(args, const char*));
    va_end(args);
  } else if (kind == TOKEN_OPERATOR) {
    va_list args;
    va_start(args, kind);
    token.optype = strintern(va_arg(args, const char*));
    va_end(args);
  } else if (kind == TOKEN_SEPARATOR) {
    va_list args;
    va_start(args, kind);
    token.optype = strintern(va_arg(args, const char*));
    va_end(args);
  }
  return token;
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
  createTest(&suite, in, 3,
             token(TOKEN_INT, 1),
             token(TOKEN_NAME, "a"),
             token(TOKEN_EOF)
            );

  in = "1_a";
  createTest(&suite, in, 3,
             token(TOKEN_INT, 1),
             token(TOKEN_NAME, "_a"),
             token(TOKEN_EOF)
            );

  in = "a 1";
  createTest(&suite, in, 3,
             token(TOKEN_NAME, "a"),
             token(TOKEN_INT, 1),
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

  in = "if else do while for switch case continue break return true false bool int var const func struct";
  createTest(&suite, in, 19,
             token(TOKEN_KEYWORD, "if"),
             token(TOKEN_KEYWORD, "else"),
             token(TOKEN_KEYWORD, "do"),
             token(TOKEN_KEYWORD, "while"),
             token(TOKEN_KEYWORD, "for"),
             token(TOKEN_KEYWORD, "switch"),
             token(TOKEN_KEYWORD, "case"),
             token(TOKEN_KEYWORD, "continue"),
             token(TOKEN_KEYWORD, "break"),
             token(TOKEN_KEYWORD, "return"),
             token(TOKEN_KEYWORD, "true"),
             token(TOKEN_KEYWORD, "false"),
             token(TOKEN_KEYWORD, "bool"),
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

  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  deleteTests();
  strinternFree();
  return result;
}
