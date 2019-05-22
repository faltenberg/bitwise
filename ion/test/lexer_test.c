#include "cunit.h"

#include "lexer.h"

#include "util.h"


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


////////////////////////////////////////////////////////////


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


static TestResult testEndOfLine() {
  TestResult result = {};

  {
    Source src = sourceFromString("");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_EOF, loc(1, 1), loc(1, 1), "")));
    t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_EOF, loc(1, 1), loc(1, 1), "")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("\0");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_EOF, loc(1, 1), loc(1, 1), "")));
    t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_EOF, loc(1, 1), loc(1, 1), "")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("\n");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_EOF, loc(2, 1), loc(2, 1), "")));
    t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_EOF, loc(2, 1), loc(2, 1), "")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("x");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_EOF, loc(1, 2), loc(1, 2), "")));
    t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_EOF, loc(1, 2), loc(1, 2), "")));
    deleteSource(&src);
  }

  return result;
}


static TestResult testTokenName() {
  TestResult result = {};

  {
    Source src = sourceFromString("x");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_NAME, loc(1, 1), loc(1, 1), "x")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("abc");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_NAME, loc(1, 1), loc(1, 3), "abc")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("ABC");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_NAME, loc(1, 1), loc(1, 3), "ABC")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("a_b");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_NAME, loc(1, 1), loc(1, 3), "a_b")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("_");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 1), "_")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("x_");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_NAME, loc(1, 1), loc(1, 2), "x_")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("_x");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_NAME, loc(1, 1), loc(1, 2), "_x")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("_123");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_NAME, loc(1, 1), loc(1, 4), "_123")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("ab123c");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_NAME, loc(1, 1), loc(1, 6), "ab123c")));
    deleteSource(&src);
  }

  return result;
}


static TestResult testTokenInt() {
  TestResult result = {};

  {
    Source src = sourceFromString("0");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_INT, loc(1, 1), loc(1, 1), "0")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("1");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_INT, loc(1, 1), loc(1, 1), "1")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("123");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_INT, loc(1, 1), loc(1, 3), "123")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("0123abc");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_ERROR, loc(1, 1), loc(1, 7), "")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("0__2_");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_INT, loc(1, 1), loc(1, 5), "0__2_")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("1__2_");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_INT, loc(1, 1), loc(1, 5), "1__2_")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("_123");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_NAME, loc(1, 1), loc(1, 4), "_123")));
    deleteSource(&src);
  }

  return result;
}


static TestResult testTokenHexInt() {
  TestResult result = {};

  {
    Source src = sourceFromString("0x12345abcdef67890");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_INT, loc(1, 1), loc(1, 18), "0x12345abcdef67890")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("0XABCDEF1234abcdef");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_INT, loc(1, 1), loc(1, 18), "0XABCDEF1234abcdef")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("1x123");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_ERROR, loc(1, 1), loc(1, 5), "")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("00x123");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_ERROR, loc(1, 1), loc(1, 6), "")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("0xABCg123");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_ERROR, loc(1, 1), loc(1, 9), "")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("0x");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_ERROR, loc(1, 1), loc(1, 2), "")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("0xABCD_1234");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_INT, loc(1, 1), loc(1, 11), "0xABCD_1234")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("0x_");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_ERROR, loc(1, 1), loc(1, 3), "")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("0X__12AB__");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_INT, loc(1, 1), loc(1, 10), "0X__12AB__")));
    deleteSource(&src);
  }

  return result;
}


static TestResult testTokenBinInt() {
  TestResult result = {};

  {
    Source src = sourceFromString("0b0101");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_INT, loc(1, 1), loc(1, 6), "0b0101")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("0B1100");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_INT, loc(1, 1), loc(1, 6), "0B1100")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("1b0011");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_ERROR, loc(1, 1), loc(1, 6), "")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("00b0011");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_ERROR, loc(1, 1), loc(1, 7), "")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("0b012");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_ERROR, loc(1, 1), loc(1, 5), "")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("0b0a1");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_ERROR, loc(1, 1), loc(1, 5), "")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("0b");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_ERROR, loc(1, 1), loc(1, 2), "")));
    deleteSource(&src);
  }

  return result;
}


static TestResult testWhitespaces() {
  TestResult result = {};

  {
    Source src = sourceFromString(" ");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_EOF, loc(1, 2), loc(1, 2), "")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("  ");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_EOF, loc(1, 3), loc(1, 3), "")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("\t");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_EOF, loc(1, 2), loc(1, 2), "")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString(" \r\n\t");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_EOF, loc(2, 2), loc(2, 2), "")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("123 xyz");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_INT, loc(1, 1), loc(1, 3), "123")));
    t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_NAME, loc(1, 5), loc(1, 7), "xyz")));
    deleteSource(&src);
  }

  return result;
}


static TestResult testTokenKeyword() {
  TestResult result = {};

  {
    Source src = sourceFromString("if");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 2), "if")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("else");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 4), "else")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("do");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 2), "do")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("while");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 5), "while")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("for");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 3), "for")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("switch");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 6), "switch")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("case");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 4), "case")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("break");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 5), "break")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("continue");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 8), "continue")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("return");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 6), "return")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("true");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 4), "true")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("false");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 5), "false")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("var");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 3), "var")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("const");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 5), "const")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("func");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 4), "func")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("struct");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 6), "struct")));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("_");
    Lexer lexer = lexerFromSource(src);
    Token t = nextToken(&lexer);
    TEST(assertEqualToken(t, token(TOKEN_KEYWORD, loc(1, 1), loc(1, 1), "_")));
    deleteSource(&src);
  }

  return result;
}


#define createTest(s, in, n, ...) __createTest(__FILE__, __LINE__, s, in, n, __VA_ARGS__)
static void __createTest(const char* file, int line, TestSuite* suite, const char* input,
                         int numTokens, ...) {
  char testName[100];
  snprintf(testName, sizeof(testName), "parse \"%s\"", input);
  printf("%s:%d %s ...\n", file, line, testName);
}


TestResult lexer_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<lexer>", "Test lexer.");
  addTest(&suite, &testCreation,     "testCreation");
  addTest(&suite, &testEndOfLine,    "testEndOfLine");
  addTest(&suite, &testTokenName,    "testTokenName");
  addTest(&suite, &testTokenInt,     "testTokenInt");
  addTest(&suite, &testTokenHexInt,  "testTokenHexInt");
  addTest(&suite, &testTokenBinInt,  "testTokenBinInt");
  addTest(&suite, &testWhitespaces,  "testWhitespaces");
  addTest(&suite, &testTokenKeyword, "testTokenKeyword");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
