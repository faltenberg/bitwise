#include "cunit/cunit.h"
#include "lexer.h"
#include "sbuffer.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


static char**       _testNames      = NULL;
static const char** _inputStreams   = NULL;
static Token**      _expectedTokens = NULL;
static size_t       _currentTest    = 0;


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


#define assertEqualToken(idx, tok, exp) __assertEqualToken(__FILE__, __LINE__, idx, tok, exp)
bool __assertEqualToken(const char* file, int line,
                        int index, const Token* token, const Token* expected) {
  printVerbose(__PROMPT, file, line);
  if (token->kind != expected->kind) {
    printVerbose(RED "ERROR: " RST);
    printVerbose("in tokens[%d] expected Token[kind='%c'(%02x)] == Token[kind='%c'(%02x)]\n",
                 index, token->kind, token->kind, expected->kind, expected->kind);
    return false;
  } else if (token->kind == TOKEN_INT && token->value != expected->value) {
    printVerbose(RED "ERROR: " RST);
    printVerbose("in tokens[%d] expected Token[TOKEN_INT(%lu)] == Token[TOKEN_INT(%lu)]\n",
                 index, token->value, expected->value);
    return false;
  } else if (token->end - token->start != expected->end - expected->start) {
    printVerbose(RED "ERROR: " RST);
    printVerbose("in tokens[%d] expected Token[\"%.*s\"] == Token[\"%.*s\"]\n",
                 index, token->end - token->start, token->start,
                 expected->end - expected->start, expected->start);
    return false;
  } else {
    bool equal = true;
    for (int i = 0; i < expected->end - expected->start + 1 && equal; i++) {
      equal = token->start[i] == expected->start[i];
    }

    if (equal) {
      printVerbose(GRN "OK\n" RST);
      return true;
    } else {
      printVerbose(RED "ERROR: " RST);
      printVerbose("in tokens[%d] expected Token[\"%.*s\"] == Token[\"%.*s\"]\n",
                   index, token->end - token->start, token->start,
                   expected->end - expected->start, expected->start);
      return false;
    }
  }
}


static TestResult test() {
  TestResult result = {};
  size_t index = _currentTest++;
  Lexer lexer = newLexer(_inputStreams[index]);
  Token* tokens = _expectedTokens[index];
  for (int i = 0; i < bufLength(tokens); i++) {
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

  bufPush(_testNames, testName);
  bufPush(_inputStreams, input);

  Token* tokens = NULL;
  va_list args;
  va_start(args, numTokens);
  for (int i = 0; i < numTokens; i++) {
    Token token = va_arg(args, Token);
    bufPush(tokens, token);
  }
  va_end(args);
  bufPush(_expectedTokens, tokens);

  addTest(suite, &test, testName);
}


static Token token(const char* input, int start, int end, TokenKind kind, ...) {
  Token token = { .kind=kind, .start=&input[start], .end=&input[end+1] };
  if (kind == TOKEN_INT) {
    va_list args;
    va_start(args, kind);
    token.value = va_arg(args, uint64_t);
    va_end(args);
  }
  return token;
}


TestResult lexer_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<lexer>", "Test lexer for ION grammar.");
  const char* in;

  in = "";
  createTest(&suite, in, 1,
             token(in, 0, 0, '\0')
            );

  in = " ";
  createTest(&suite, in, 1,
             token(in, 1, 1, '\0')
            );

  in = "1";
  createTest(&suite, in, 2,
             token(in, 0, 0, TOKEN_INT, 1),
             token(in, 1, 1, '\0')
            );

  in = "234";
  createTest(&suite, in, 2,
             token(in, 0, 2, TOKEN_INT, 234),
             token(in, 3, 3, '\0')
            );

  in = "0567";
  createTest(&suite, in, 2,
             token(in, 0, 3, TOKEN_INT, 567),
             token(in, 4, 4, '\0')
            );

  in = " 8";
  createTest(&suite, in, 2,
             token(in, 1, 1, TOKEN_INT, 8),
             token(in, 2, 2, '\0')
            );

  in = "9 ";
  createTest(&suite, in, 2,
             token(in, 0, 0, TOKEN_INT, 9),
             token(in, 2, 2, '\0')
            );

  in = "a";
  createTest(&suite, in, 2,
             token(in, 0, 0, TOKEN_NAME),
             token(in, 1, 1, '\0')
            );

  in = "abcdefghijklmnopqrstuvwxyz";
  createTest(&suite, in, 2,
             token(in, 0, 25, TOKEN_NAME),
             token(in, 26, 26, '\0')
            );

  in = "_";
  createTest(&suite, in, 2,
             token(in, 0, 0, TOKEN_NAME),
             token(in, 1, 1, '\0')
            );

  in = "_a1";
  createTest(&suite, in, 2,
             token(in, 0, 2, TOKEN_NAME),
             token(in, 3, 3, '\0')
            );

  in = "a1_";
  createTest(&suite, in, 2,
             token(in, 0, 2, TOKEN_NAME),
             token(in, 3, 3, '\0')
            );

  in = "a_1";
  createTest(&suite, in, 2,
             token(in, 0, 2, TOKEN_NAME),
             token(in, 3, 3, '\0')
            );

  in = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  createTest(&suite, in, 2,
             token(in, 0, 25, TOKEN_NAME),
             token(in, 26, 26, '\0')
            );

  in = "aBc";
  createTest(&suite, in, 2,
             token(in, 0, 2, TOKEN_NAME),
             token(in, 3, 3, '\0')
            );

  in = "Abc";
  createTest(&suite, in, 2,
             token(in, 0, 2, TOKEN_NAME),
             token(in, 3, 3, '\0')
            );

  in = "abC";
  createTest(&suite, in, 2,
             token(in, 0, 2, TOKEN_NAME),
             token(in, 3, 3, '\0')
            );

  in = "1 + 1";
  createTest(&suite, in, 4,
             token(in, 0, 0, TOKEN_INT, 1),
             token(in, 2, 2, '+'),
             token(in, 4, 4, TOKEN_INT, 1),
             token(in, 5, 5, '\0')
            );

  in = "a + b";
  createTest(&suite, in, 4,
             token(in, 0, 0, TOKEN_NAME),
             token(in, 2, 2, '+'),
             token(in, 4, 4, TOKEN_NAME),
             token(in, 5, 5, '\0')
            );

  in = "(1)";
  createTest(&suite, in, 4,
             token(in, 0, 0, '('),
             token(in, 1, 1, TOKEN_INT, 1),
             token(in, 2, 2, ')'),
             token(in, 3, 3, '\0')
            );

  in = "a[1]";
  createTest(&suite, in, 4,
             token(in, 0, 0, TOKEN_NAME),
             token(in, 1, 1, '['),
             token(in, 2, 2, TOKEN_INT, 1),
             token(in, 3, 3, ']'),
             token(in, 4, 4, '\0')
            );

  in = "i++";
  createTest(&suite, in, 3,
             token(in, 0, 0, TOKEN_NAME),
             token(in, 1, 2, TOKEN_OPERATOR),
             token(in, 3, 3, '\0')
            );

  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  deleteTests();
  return result;
}
