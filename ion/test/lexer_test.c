#include "cunit.h"

#include "lexer.h"

#include "util.h"
#include "sbuffer.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>


static TokenLoc loc(int line, int pos) {
  return (TokenLoc){ .line=line, .pos=pos };
}


static Token token(TokenKind kind, TokenLoc start, TokenLoc end, const char* chars) {
  return (Token){ .kind=kind, .source=NULL, .start=start, .end=end,
                  .chars=stringFromArray(chars) };
}


static bool equalLoc(TokenLoc loc, TokenLoc exp) {
  return loc.line == exp.line && loc.pos == exp.pos;
}


#define assertEqualTokenLoc(tl, exp)  __assertEqualTokenLoc(__FILE__, __LINE__, tl, exp)
static bool __assertEqualTokenLoc(const char* file, int line, TokenLoc loc, TokenLoc exp) {
  printVerbose(__PROMPT, file, line);

  if (equalLoc(loc, exp)) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected TokenLoc [%d:%d] == [%d:%d]\n",
                 loc.line, loc.pos, exp.line, exp.pos);
    return false;
  }
}


#define assertEqualToken(t, exp)  __assertEqualToken(__FILE__, __LINE__, t, exp)
static bool __assertEqualToken(const char* file, int line, Token t, Token exp) {
  printVerbose(__PROMPT, file, line);

  if (t.kind != exp.kind) {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected Token [%s] == [%s]\n", str(t.kind).chars, str(exp.kind).chars);
    return false;
  }

  if (!equalLoc(t.start, exp.start)) {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected TokenLoc [%d:%d] == [%d:%d]\n",
                 t.start.line, t.start.pos, exp.start.line, exp.start.pos);
    return false;
  }
  if (!equalLoc(t.end, exp.end)) {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected TokenLoc [%d:%d] == [%d:%d]\n",
                 t.end.line, t.end.pos, exp.end.line, exp.end.pos);
    return false;
  }

  printVerbose(GRN "OK\n" RST);

  if (exp.kind == TOKEN_ERROR) {
    return true;
  }

  return __assertEqualStr(file, line, t.chars, exp.chars.chars);
}


typedef struct TestCase {
  char*       name;
  char*       input;
  const char* file;
  int         line;
  SBUF(Token) tokens;
} TestCase;


static SBUF(TestCase) g_testCases = NULL;
static int            g_current = 0;


static TestResult testFunc() {
  TestResult result = {};
  TestCase* testCase = &g_testCases[g_current++];
  Source src = sourceFromString(testCase->input);
  Lexer lexer = lexerFromSource(src);

  for (int i = 0; i < sbufLength(testCase->tokens); i++) {
    Token t = nextToken(&lexer);
    TEST(__assertEqualToken(testCase->file, testCase->line, t, testCase->tokens[i]));
  }

  deleteSource(&src);
  return result;
}


static char* unescape(const char* in) {
  char* out = (char*) malloc(strlen(in) + 1);

  bool esc = false;
  int i = 0;
  while (*in != '\0') {
    if (esc) {
      esc = false;

      switch (*in) {
        case '\\':  out[i++] = '\\';  break;
        case '\'':  out[i++] = '\'';  break;
        case '\"':  out[i++] = '\"';  break;
        case 'r':   out[i++] = '\r';  break;
        case 'n':   out[i++] = '\n';  break;
        case 't':   out[i++] = '\t';  break;
        case 'v':   out[i++] = '\v';  break;
        case '0':   out[i++] = '\0';  break;
        case 'a':   out[i++] = '\a';  break;
        case 'b':   out[i++] = '\b';  break;
        case 'e':   out[i++] = '\e';  break;
        case 'f':   out[i++] = '\f';  break;
      }
    } else if (*in == '\\') {
      esc = true;
    } else {
      out[i++] = *in;
    }
    in++;
  }

  out[i] = '\0';
  return out;
}


#define createTest(s, in, n, ...) __createTest(__FILE__, __LINE__, s, in, n, __VA_ARGS__)
static void __createTest(const char* file, int line, TestSuite* suite, const char* input,
                         int numTokens, ...) {
  TestCase testCase = { .file=file, .line=line };
  testCase.name = (char*) malloc(100);
  snprintf(testCase.name, 100, "parse \"%s\"", input);
  testCase.input = unescape(input);

  va_list args;
  va_start(args, numTokens);
  for (int i = 0; i < numTokens; i++) {
    Token token = va_arg(args, Token);
    sbufPush(testCase.tokens, token);
  }
  va_end(args);

  sbufPush(g_testCases, testCase);
  addTest(suite, &testFunc, testCase.name);
}


/********************************************* TESTS *********************************************/


static TestResult testCreation() {
  TestResult result = {};

  {
    Source src = sourceFromString("foo bar");
    Lexer lexer = lexerFromSource(src);
    TEST(assertEqualStr(lexer.source.content, "foo bar"));
    TEST(assertEqualInt(lexer.index, 0));
    TEST(assertEqualTokenLoc(lexer.currentLoc, loc(0, 0)));
    TEST(assertEqualTokenLoc(lexer.nextLoc, loc(1, 1)));
    deleteSource(&src);
  }

  return result;
}


static void addTestsEndOfLine(TestSuite* suite) {
  const char* in;

  in = "";
  createTest(suite, in, 2,
             token(TOKEN_EOF, loc(1, 1), loc(1, 1), ""),
             token(TOKEN_EOF, loc(1, 1), loc(1, 1), "")
            );

  in = "\\0";
  createTest(suite, in, 2,
             token(TOKEN_EOF, loc(1, 1), loc(1, 1), ""),
             token(TOKEN_EOF, loc(1, 1), loc(1, 1), "")
            );

  in = "\\n";
  createTest(suite, in, 2,
             token(TOKEN_EOF, loc(2, 1), loc(2, 1), ""),
             token(TOKEN_EOF, loc(2, 1), loc(2, 1), "")
            );

  in = "x";
  createTest(suite, in, 3,
             token(TOKEN_NAME, loc(1, 1), loc(1, 1), "x"),
             token(TOKEN_EOF, loc(1, 2), loc(1, 2), ""),
             token(TOKEN_EOF, loc(1, 2), loc(1, 2), "")
            );
}


static void addTestsTokenName(TestSuite* suite) {
  const char* in;

  in = "x";
  createTest(suite, in, 1,
             token(TOKEN_NAME, loc(1, 1), loc(1, 1), "x")
            );

  in = "abc";
  createTest(suite, in, 1,
             token(TOKEN_NAME, loc(1, 1), loc(1, 3), "abc")
            );

  in = "ABC";
  createTest(suite, in, 1,
             token(TOKEN_NAME, loc(1, 1), loc(1, 3), "ABC")
            );

  in = "a_b";
  createTest(suite, in, 1,
             token(TOKEN_NAME, loc(1, 1), loc(1, 3), "a_b")
            );

  in = "_";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 1), "_")
            );

  in = "x_";
  createTest(suite, in, 1,
             token(TOKEN_NAME, loc(1, 1), loc(1, 2), "x_")
            );

  in = "_x";
  createTest(suite, in, 1,
             token(TOKEN_NAME, loc(1, 1), loc(1, 2), "_x")
            );

  in = "_123";
  createTest(suite, in, 1,
             token(TOKEN_NAME, loc(1, 1), loc(1, 4), "_123")
            );

  in = "ab123c";
  createTest(suite, in, 1,
             token(TOKEN_NAME, loc(1, 1), loc(1, 6), "ab123c")
            );
}


static void addTestsTokenInt(TestSuite* suite) {
  const char* in;

  in = "0";
  createTest(suite, in, 1,
             token(TOKEN_INT, loc(1, 1), loc(1, 1), "0")
            );

  in = "1";
  createTest(suite, in, 1,
             token(TOKEN_INT, loc(1, 1), loc(1, 1), "1")
            );

  in = "123";
  createTest(suite, in, 1,
             token(TOKEN_INT, loc(1, 1), loc(1, 3), "123")
            );

  in = "0123abc";
  createTest(suite, in, 1,
             token(TOKEN_ERROR, loc(1, 1), loc(1, 7), "")
            );

  in = "0__2_";
  createTest(suite, in, 1,
             token(TOKEN_INT, loc(1, 1), loc(1, 5), "0__2_")
            );

  in = "1__2_";
  createTest(suite, in, 1,
             token(TOKEN_INT, loc(1, 1), loc(1, 5), "1__2_")
            );

  in = "_123";
  createTest(suite, in, 1,
             token(TOKEN_NAME, loc(1, 1), loc(1, 4), "_123")
            );
}


static void addTestsTokenHexInt(TestSuite* suite) {
  const char* in;

  in = "0x12345abcdef67890";
  createTest(suite, in, 1,
             token(TOKEN_INT, loc(1, 1), loc(1, 18), "0x12345abcdef67890")
            );

  in = "0XABCDEF1234abcdef";
  createTest(suite, in, 1,
             token(TOKEN_INT, loc(1, 1), loc(1, 18), "0XABCDEF1234abcdef")
            );

  in = "1x123";
  createTest(suite, in, 1,
             token(TOKEN_ERROR, loc(1, 1), loc(1, 5), "")
            );

  in = "00x123";
  createTest(suite, in, 1,
             token(TOKEN_ERROR, loc(1, 1), loc(1, 6), "")
            );

  in = "0xABCg123";
  createTest(suite, in, 1,
             token(TOKEN_ERROR, loc(1, 1), loc(1, 9), "")
            );

  in = "0x";
  createTest(suite, in, 1,
             token(TOKEN_ERROR, loc(1, 1), loc(1, 2), "")
            );

  in = "0xABCD_1234";
  createTest(suite, in, 1,
             token(TOKEN_INT, loc(1, 1), loc(1, 11), "0xABCD_1234")
            );

  in = "0x_";
  createTest(suite, in, 1,
             token(TOKEN_ERROR, loc(1, 1), loc(1, 3), "0x_")
            );

  in = "0X__12AB__";
  createTest(suite, in, 1,
             token(TOKEN_INT, loc(1, 1), loc(1, 10), "0X__12AB__")
            );
}


static void addTestsTokenBinInt(TestSuite* suite) {
  const char* in;

  in = "0b0101";
  createTest(suite, in, 1,
             token(TOKEN_INT, loc(1, 1), loc(1, 6), "0b0101")
            );

  in = "0B1100";
  createTest(suite, in, 1,
             token(TOKEN_INT, loc(1, 1), loc(1, 6), "0B1100")
            );

  in = "1b0011";
  createTest(suite, in, 1,
             token(TOKEN_ERROR, loc(1, 1), loc(1, 6), "")
            );

  in = "00b0011";
  createTest(suite, in, 1,
             token(TOKEN_ERROR, loc(1, 1), loc(1, 7), "")
            );

  in = "0b012";
  createTest(suite, in, 1,
             token(TOKEN_ERROR, loc(1, 1), loc(1, 5), "")
            );

  in = "0b0a1";
  createTest(suite, in, 1,
             token(TOKEN_ERROR, loc(1, 1), loc(1, 5), "")
            );

  in = "0b";
  createTest(suite, in, 1,
             token(TOKEN_ERROR, loc(1, 1), loc(1, 2), "")
            );
}


static void addTestsWhitespaces(TestSuite* suite) {
  const char* in;

  in = " ";
  createTest(suite, in, 1,
             token(TOKEN_EOF, loc(1, 2), loc(1, 2), "")
            );

  in = "  ";
  createTest(suite, in, 1,
             token(TOKEN_EOF, loc(1, 3), loc(1, 3), "")
            );

  in = "\\t";
  createTest(suite, in, 1,
             token(TOKEN_EOF, loc(1, 2), loc(1, 2), "")
            );

  in = " \\r\\n\\t";
  createTest(suite, in, 1,
             token(TOKEN_EOF, loc(2, 2), loc(2, 2), "")
            );

  in = "123 xyz";
  createTest(suite, in, 2,
             token(TOKEN_INT, loc(1, 1), loc(1, 3), "123"),
             token(TOKEN_NAME, loc(1, 5), loc(1, 7), "xyz")
            );
}


static void addTestsTokenKeyword(TestSuite* suite) {
  const char* in;

  in = "if";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 2), "if")
            );

  in = "else";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 4), "else")
            );

  in = "do";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 2), "do")
            );

  in = "while";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 5), "while")
            );

  in = "for";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 3), "for")
            );

  in = "switch";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 6), "switch")
            );

  in = "case";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 4), "case")
            );

  in = "break";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 5), "break")
            );

  in = "continue";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 8), "continue")
            );

  in = "return";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 6), "return")
            );

  in = "true";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 4), "true")
            );

  in = "false";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 5), "false")
            );

  in = "var";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 3), "var")
            );

  in = "const";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 5), "const")
            );

  in = "func";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 4), "func")
            );

  in = "struct";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 6), "struct")
            );

  in = "_";
  createTest(suite, in, 1,
             token(TOKEN_KEYWORD, loc(1, 1), loc(1, 1), "_")
            );
}


TestResult lexer_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<lexer>", "Test lexer.");
  addTest(&suite, &testCreation, "testCreation");
  addTestsEndOfLine(&suite);
  addTestsTokenName(&suite);
  addTestsTokenInt(&suite);
  addTestsTokenHexInt(&suite);
  addTestsTokenBinInt(&suite);
  addTestsWhitespaces(&suite);
  addTestsTokenKeyword(&suite);
  TestResult result = run(&suite, verbosity);

  deleteSuite(&suite);
  for (int i = 0; i < sbufLength(g_testCases); i++) {
    free(g_testCases[i].name);
    free(g_testCases[i].input);
    sbufFree(g_testCases[i].tokens);
  }
  sbufFree(g_testCases);

  return result;
}
