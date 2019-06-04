#include "cunit.h"
#include "util.h"

#include "lexer.h"

#include "sbuffer.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>


GENERATE_ASSERT_EQUAL_ENUM(TokenKind)


#define msg(loc, msg, line, spaces, indicator) \
        "<cstring>:"loc": \e[31mError:\e[39m "msg"\n"line"\n"spaces"\e[32m"indicator"\e[39m\n"


static Error* error(Location loc, const char* message) {
  Error* error = (Error*) malloc(sizeof(Error));
  error->location = loc;
  error->message = stringFromArray(message);
  error->cause = NULL;
  return error;
}


static Token token(TokenKind kind, Location start, Location end, const char* chars) {
  return (Token){ .kind=kind, .source=NULL, .start=start, .end=end,
                  .chars=stringFromArray(chars), .error=NULL
                };
}


static Token tokenError(Location start, Location end, const char* chars, Location errorLoc,
                        const char* message) {
  return (Token){ .kind=TOKEN_ERROR, .source=NULL, .start=start, .end=end,
                  .chars=stringFromArray(chars), .error=error(errorLoc, message)
                };
}


static bool equalLoc(Location loc, Location exp) {
  return loc.line == exp.line && loc.pos == exp.pos;
}


#define assertEqualLocation(tl, exp)  __assertEqualLocation(__FILE__, __LINE__, tl, exp)
static bool __assertEqualLocation(const char* file, int line, Location loc, Location exp) {
  printVerbose(__PROMPT, file, line);

  if (equalLoc(loc, exp)) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected Location [%d:%d] == [%d:%d]\n",
                 loc.line, loc.pos, exp.line, exp.pos);
    return false;
  }
}


#define assertEqualToken(t, exp)  __assertEqualToken(__FILE__, __LINE__, t, exp)
static bool __assertEqualToken(const char* file, int line, Token t, Token exp) {
  bool equal = assertEqualEnum(TokenKind, t.kind, exp.kind);
  if (!equal) {
    return false;
  }

  printVerbose(__PROMPT, file, line);
  if (!equalLoc(t.start, exp.start)) {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected Location [%d:%d] == [%d:%d]\n",
                 t.start.line, t.start.pos, exp.start.line, exp.start.pos);
    return false;
  }
  if (!equalLoc(t.end, exp.end)) {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected Location [%d:%d] == [%d:%d]\n",
                 t.end.line, t.end.pos, exp.end.line, exp.end.pos);
    return false;
  }
  printVerbose(GRN "OK\n" RST);

  equal = __assertEqualStr(file, line, t.chars, exp.chars.chars);
  if (!equal) {
    return false;
  }

  if (exp.kind != TOKEN_ERROR) {
    return true;
  }

  equal = __assertNotNull(file, line, t.error);
  if (!equal) {
    return false;
  }

  printVerbose(__PROMPT, file, line);
  if (!equalLoc(t.error->location, exp.error->location)) {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected Location [%d:%d] == [%d:%d]\n",
                 t.error->location.line, t.error->location.pos,
                 exp.error->location.line, exp.error->location.pos);
    return false;
  }
  printVerbose(GRN "OK\n" RST);

  return __assertEqualStr(file, line, t.error->message, exp.error->message.chars);
}


typedef struct TestCase {
  const char* file;
  int         line;
  char*       name;
  char*       input;
  SBUF(Token) tokens;
} TestCase;


static SBUF(TestCase) g_testCases = NULL;
static int            g_current = 0;


static TestResult testFunc() {
  TestResult result = {};
  TestCase* testCase = &g_testCases[g_current++];
  Source src = sourceFromString(testCase->input);
  Lexer lexer = lexerFromSource(&src);

  for (int i = 0; i < sbufLength(testCase->tokens); i++) {
    Token token = nextToken(&lexer);
    Token exp = testCase->tokens[i];

    /* Assume that all tests are created as
     * addTest(&suite, numTok,
     *   token(...)
     * );
     *
     * Then testCase->line is the line with ");". By subtracting numTok and adding i, we can pass
     * the correct line to __assertEqualToken(). If numTok is wrong, then a wrong line is passed.
     */
    int line = testCase->line - sbufLength(testCase->tokens) + i;
    TEST(__assertEqualToken(testCase->file, line, token, exp));
    if (token.kind == TOKEN_ERROR) {
      deleteError(token.error);
      free(token.error);
    }
    if (exp.kind == TOKEN_ERROR) {
      deleteError(exp.error);
      free(exp.error);
    }
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
    Lexer lexer = lexerFromSource(&src);
    TEST(assertEqualStr(lexer.source->content, "foo bar"));
    TEST(assertEqualInt(lexer.index, 0));
    TEST(assertEqualChar(lexer.currentChar, 0));
    TEST(assertEqualLocation(lexer.currentLoc, loc(0, 0)));
    TEST(assertEqualLocation(lexer.nextLoc, loc(1, 1)));
    deleteSource(&src);
  }

  return result;
}


static TestResult testErrorMsgs() {
  TestResult result = {};

  {
// TODO
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

  in = " ";
  createTest(suite, in, 2,
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
    tokenError(loc(1, 1), loc(1, 7), "0123abc", loc(1, 5),
               msg("1:5", "invalid integer format", "0123abc", "", "~~~~^~~"))
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
    tokenError(loc(1, 1), loc(1, 5), "1x123", loc(1, 2),
               msg("1:2", "invalid integer format", "1x123", "", "~^~~~"))
  );

  in = "00x123";
  createTest(suite, in, 1,
    tokenError(loc(1, 1), loc(1, 6), "00x123", loc(1, 3),
               msg("1:3", "invalid integer format", "00x123", "", "~~^~~~"))
  );

  in = "0xABCg123";
  createTest(suite, in, 1,
    tokenError(loc(1, 1), loc(1, 9), "0xABCg123", loc(1, 6),
               msg("1:6", "invalid hex integer format", "0xABCg123", "", "~~~~~^~~~"))
  );

  in = "0x";
  createTest(suite, in, 1,
    tokenError(loc(1, 1), loc(1, 2), "0x", loc(1, 2),
               msg("1:2", "hex integer must have at least one digit", "0x", "", "~^"))
  );

  in = "0xABCD_1234";
  createTest(suite, in, 1,
    token(TOKEN_INT, loc(1, 1), loc(1, 11), "0xABCD_1234")
  );

  in = "0x_";
  createTest(suite, in, 1,
    tokenError(loc(1, 1), loc(1, 3), "0x_", loc(1, 3),
               msg("1:3", "hex integer must have at least one digit", "0x_", "", "~~^"))
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
    tokenError(loc(1, 1), loc(1, 6), "1b0011", loc(1, 2),
               msg("1:2", "invalid integer format", "1b0011", "", "~^~~~~"))
  );

  in = "00b0011";
  createTest(suite, in, 1,
    tokenError(loc(1, 1), loc(1, 7), "00b0011", loc(1, 3),
               msg("1:3", "invalid integer format", "00b0011", "", "~~^~~~~"))
  );

  in = "0b012";
  createTest(suite, in, 1,
    tokenError(loc(1, 1), loc(1, 5), "0b012", loc(1, 5),
               msg("1:5", "invalid bin integer format", "0b012", "", "~~~~^"))
  );

  in = "0b0a1";
  createTest(suite, in, 1,
    tokenError(loc(1, 1), loc(1, 5), "0b0a1", loc(1, 4),
               msg("1:4", "invalid bin integer format", "0b0a1", "", "~~~^~"))
  );

  in = "0b";
  createTest(suite, in, 1,
    tokenError(loc(1, 1), loc(1, 2), "0b", loc(1, 2),
               msg("1:2", "bin integer must have at least one digit", "0b", "", "~^"))
  );

  in = "0b_";
  createTest(suite, in, 1,
    tokenError(loc(1, 1), loc(1, 3), "0b_", loc(1, 3),
               msg("1:3", "bin integer must have at least one digit", "0b_", "", "~~^"))
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

  in = "\\r";
  createTest(suite, in, 1,
    token(TOKEN_EOF, loc(1, 2), loc(1, 2), "")
  );

  in = "\\n";
  createTest(suite, in, 1,
    token(TOKEN_EOF, loc(2, 1), loc(2, 1), "")
  );

  in = "\\r\\n";
  createTest(suite, in, 1,
    token(TOKEN_EOF, loc(2, 1), loc(2, 1), "")
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


static void addTestsTokenSeparator(TestSuite* suite) {
  const char* in;

  in = "(";
  createTest(suite, in, 1,
    token(TOKEN_SYMBOL, loc(1, 1), loc(1, 1), "(")
  );

  in = ")";
  createTest(suite, in, 1,
    token(TOKEN_SYMBOL, loc(1, 1), loc(1, 1), ")")
  );

  in = "[";
  createTest(suite, in, 1,
    token(TOKEN_SYMBOL, loc(1, 1), loc(1, 1), "[")
  );

  in = "]";
  createTest(suite, in, 1,
    token(TOKEN_SYMBOL, loc(1, 1), loc(1, 1), "]")
  );

  in = "{";
  createTest(suite, in, 1,
    token(TOKEN_SYMBOL, loc(1, 1), loc(1, 1), "{")
  );

  in = "}";
  createTest(suite, in, 1,
    token(TOKEN_SYMBOL, loc(1, 1), loc(1, 1), "}")
  );

  in = ",";
  createTest(suite, in, 1,
    token(TOKEN_SYMBOL, loc(1, 1), loc(1, 1), ",")
  );

  in = ";";
  createTest(suite, in, 1,
    token(TOKEN_SYMBOL, loc(1, 1), loc(1, 1), ";")
  );

  in = ":";
  createTest(suite, in, 1,
    token(TOKEN_SYMBOL, loc(1, 1), loc(1, 1), ":")
  );

  in = ".";
  createTest(suite, in, 1,
    token(TOKEN_SYMBOL, loc(1, 1), loc(1, 1), ".")
  );
}


static void addTestsTokenOperator(TestSuite* suite) {
  const char* in;

  in = "+";
  createTest(suite, in, 1,
    token(TOKEN_SYMBOL, loc(1, 1), loc(1, 1), "+")
  );

  in = "++";
  createTest(suite, in, 1,
    token(TOKEN_SYMBOL, loc(1, 1), loc(1, 1), "+"),
    token(TOKEN_SYMBOL, loc(1, 2), loc(1, 2), "+")
  );

  in = "-";
  createTest(suite, in, 1,
    token(TOKEN_SYMBOL, loc(1, 1), loc(1, 1), "-")
  );

  in = "--";
  createTest(suite, in, 1,
    token(TOKEN_SYMBOL, loc(1, 1), loc(1, 1), "-"),
    token(TOKEN_SYMBOL, loc(1, 2), loc(1, 2), "-")
  );

  in = "*";
  createTest(suite, in, 1,
    token(TOKEN_SYMBOL, loc(1, 1), loc(1, 1), "*")
  );

  in = "/";
  createTest(suite, in, 1,
    token(TOKEN_SYMBOL, loc(1, 1), loc(1, 1), "/")
  );

  in = "%";
  createTest(suite, in, 1,
    token(TOKEN_SYMBOL, loc(1, 1), loc(1, 1), "%")
  );
}


static void addTestsTokenComment(TestSuite* suite) {
  const char* in;

  in = "//";
  createTest(suite, in, 1,
    token(TOKEN_COMMENT, loc(1, 1), loc(1, 2), "//")
  );

  in = "// x";
  createTest(suite, in, 1,
    token(TOKEN_COMMENT, loc(1, 1), loc(1, 4), "// x")
  );

  in = "//\\n";
  createTest(suite, in, 1,
    token(TOKEN_COMMENT, loc(1, 1), loc(1, 2), "//")
  );

  in = "// //";
  createTest(suite, in, 1,
    token(TOKEN_COMMENT, loc(1, 1), loc(1, 5), "// //")
  );

  in = "//\\nx";
  createTest(suite, in, 1,
    token(TOKEN_COMMENT, loc(1, 1), loc(1, 2), "//")
  );

  in = "/* */";
  createTest(suite, in, 1,
    token(TOKEN_COMMENT, loc(1, 1), loc(1, 5), "/* */")
  );

  in = "/*";
  createTest(suite, in, 1,
    tokenError(loc(1, 1), loc(1, 2), "/*", loc(1, 1),
               msg("1:1", "unclosed multi-line comment", "/*", "", "^~"))
  );

  in = "/**";
  createTest(suite, in, 1,
    tokenError(loc(1, 1), loc(1, 3), "/**", loc(1, 1),
               msg("1:1", "unclosed multi-line comment", "/**", "", "^~~"))
  );

  in = "/**/";
  createTest(suite, in, 1,
    token(TOKEN_COMMENT, loc(1, 1), loc(1, 4), "/**/")
  );

  in = "/*/";
  createTest(suite, in, 1,
    tokenError(loc(1, 1), loc(1, 3), "/*/", loc(1, 1),
               msg("1:1", "unclosed multi-line comment", "/*/", "", "^~~"))
  );

  in = "/** /";
  createTest(suite, in, 1,
    tokenError(loc(1, 1), loc(1, 5), "/** /", loc(1, 1),
               msg("1:1", "unclosed multi-line comment", "/** /", "", "^~~~~"))
  );

  in = "/*\\n*/";
  createTest(suite, in, 1,
    token(TOKEN_COMMENT, loc(1, 1), loc(2, 2), "/*\n*/")
  );

  in = "/*\\n";
  createTest(suite, in, 1,
    tokenError(loc(1, 1), loc(1, 3), "/*\n", loc(1, 1),
               msg("1:1", "unclosed multi-line comment", "/*", "", "^~~"))
  );

  in = "/* *\\n/";
  createTest(suite, in, 1,
    tokenError(loc(1, 1), loc(2, 1), "/* *\n/", loc(1, 1),
               msg("1:1", "unclosed multi-line comment", "/* *", "", "^~~~~"))
  );

  in = "/*//*/";
  createTest(suite, in, 1,
    token(TOKEN_COMMENT, loc(1, 1), loc(1, 6), "/*//*/")
  );

  in = "/*/**/";
  createTest(suite, in, 1,
    token(TOKEN_COMMENT, loc(1, 1), loc(1, 6), "/*/**/")
  );

  in = "/**/*/";
  createTest(suite, in, 1,
    token(TOKEN_COMMENT, loc(1, 1), loc(1, 4), "/**/")
  );

  in = "/*/**/*/";
  createTest(suite, in, 1,
    token(TOKEN_COMMENT, loc(1, 1), loc(1, 6), "/*/**/")
  );

  in = "// /**/";
  createTest(suite, in, 1,
    token(TOKEN_COMMENT, loc(1, 1), loc(1, 7), "// /**/")
  );

  in = "// /*\\n*/";
  createTest(suite, in, 1,
    token(TOKEN_COMMENT, loc(1, 1), loc(1, 5), "// /*")
  );

  in = "/*\\n//*/";
  createTest(suite, in, 1,
    token(TOKEN_COMMENT, loc(1, 1), loc(2, 4), "/*\n//*/")
  );
}


static void addTestsDeclarations(TestSuite* suite) {
  const char* in;

  in = "var x : int;  // x = 0";
  createTest(suite, in, 7,
    token(TOKEN_KEYWORD, loc(1,  1), loc(1,  3), "var"),
    token(TOKEN_NAME,    loc(1,  5), loc(1,  5), "x"),
    token(TOKEN_SYMBOL,  loc(1,  7), loc(1,  7), ":"),
    token(TOKEN_NAME,    loc(1,  9), loc(1, 11), "int"),
    token(TOKEN_SYMBOL,  loc(1, 12), loc(1, 12), ";"),
    token(TOKEN_COMMENT, loc(1, 15), loc(1, 22), "// x = 0"),
    token(TOKEN_EOF,     loc(1, 23), loc(1, 23), "")
  );

  in = "var x : int = 123;  /* x = 123 */";
  createTest(suite, in, 9,
    token(TOKEN_KEYWORD, loc(1,  1), loc(1,  3), "var"),
    token(TOKEN_NAME,    loc(1,  5), loc(1,  5), "x"),
    token(TOKEN_SYMBOL,  loc(1,  7), loc(1,  7), ":"),
    token(TOKEN_NAME,    loc(1,  9), loc(1, 11), "int"),
    token(TOKEN_SYMBOL,  loc(1, 13), loc(1, 13), "="),
    token(TOKEN_INT,     loc(1, 15), loc(1, 17), "123"),
    token(TOKEN_SYMBOL,  loc(1, 18), loc(1, 18), ";"),
    token(TOKEN_COMMENT, loc(1, 21), loc(1, 33), "/* x = 123 */"),
    token(TOKEN_EOF,     loc(1, 34), loc(1, 34), "")
  );

  in = "var mask := 0b1010_1011;";
  createTest(suite, in, 7,
    token(TOKEN_KEYWORD, loc(1,  1), loc(1,  3), "var"),
    token(TOKEN_NAME,    loc(1,  5), loc(1,  8), "mask"),
    token(TOKEN_SYMBOL,  loc(1, 10), loc(1, 10), ":"),
    token(TOKEN_SYMBOL,  loc(1, 11), loc(1, 11), "="),
    token(TOKEN_INT,     loc(1, 13), loc(1, 23), "0b1010_1011"),
    token(TOKEN_SYMBOL,  loc(1, 24), loc(1, 24), ";"),
    token(TOKEN_EOF,     loc(1, 25), loc(1, 25), "")
  );

  in = "const ADDR := 0x_AB40_;";
  createTest(suite, in, 7,
    token(TOKEN_KEYWORD, loc(1,  1), loc(1,  5), "const"),
    token(TOKEN_NAME,    loc(1,  7), loc(1, 10), "ADDR"),
    token(TOKEN_SYMBOL,  loc(1, 12), loc(1, 12), ":"),
    token(TOKEN_SYMBOL,  loc(1, 13), loc(1, 13), "="),
    token(TOKEN_INT,     loc(1, 15), loc(1, 22), "0x_AB40_"),
    token(TOKEN_SYMBOL,  loc(1, 23), loc(1, 23), ";"),
    token(TOKEN_EOF,     loc(1, 24), loc(1, 24), "")
  );

  in = "var a_1 := 0x_;";
  createTest(suite, in, 7,
    token(TOKEN_KEYWORD, loc(1,  1), loc(1,  3), "var"),
    token(TOKEN_NAME,    loc(1,  5), loc(1,  7), "a_1"),
    token(TOKEN_SYMBOL,  loc(1,  9), loc(1,  9), ":"),
    token(TOKEN_SYMBOL,  loc(1, 10), loc(1, 10), "="),
    tokenError(loc(1, 12), loc(1, 14), "0x_", loc(1, 14),
               msg("1:14", "hex integer must have at least one digit",
                   "var a_1 := 0x_;", "           ", "~~^")),
    token(TOKEN_SYMBOL,  loc(1, 15), loc(1, 15), ";"),
    token(TOKEN_EOF,     loc(1, 16), loc(1, 16), "")
  );

  in = "func f : () -> int {\\n return -1;\\n }";
  createTest(suite, in, 14,
    token(TOKEN_KEYWORD, loc(1,  1), loc(1,  4), "func"),
    token(TOKEN_NAME,    loc(1,  6), loc(1,  6), "f"),
    token(TOKEN_SYMBOL,  loc(1,  8), loc(1,  8), ":"),
    token(TOKEN_SYMBOL,  loc(1, 10), loc(1, 10), "("),
    token(TOKEN_SYMBOL,  loc(1, 11), loc(1, 11), ")"),
    token(TOKEN_SYMBOL,  loc(1, 13), loc(1, 14), "->"),
    token(TOKEN_NAME,    loc(1, 16), loc(1, 18), "int"),
    token(TOKEN_SYMBOL,  loc(1, 20), loc(1, 20), "{"),
    token(TOKEN_KEYWORD, loc(2,  2), loc(2,  7), "return"),
    token(TOKEN_SYMBOL,  loc(2,  9), loc(2,  9), "-"),
    token(TOKEN_INT,     loc(2, 10), loc(2, 10), "1"),
    token(TOKEN_SYMBOL,  loc(2, 11), loc(2, 11), ";"),
    token(TOKEN_SYMBOL,  loc(3,  2), loc(3,  2), "}"),
    token(TOKEN_EOF,     loc(3,  3), loc(3,  3), "")
  );

  in = "func f : (a: int, b: int) -> int { return a+b; }";
  createTest(suite, in, 22,
    token(TOKEN_KEYWORD, loc(1,  1), loc(1,  4), "func"),
    token(TOKEN_NAME,    loc(1,  6), loc(1,  6), "f"),
    token(TOKEN_SYMBOL,  loc(1,  8), loc(1,  8), ":"),
    token(TOKEN_SYMBOL,  loc(1, 10), loc(1, 10), "("),
    token(TOKEN_NAME,    loc(1, 11), loc(1, 11), "a"),
    token(TOKEN_SYMBOL,  loc(1, 12), loc(1, 12), ":"),
    token(TOKEN_NAME,    loc(1, 14), loc(1, 16), "int"),
    token(TOKEN_SYMBOL,  loc(1, 17), loc(1, 17), ","),
    token(TOKEN_NAME,    loc(1, 19), loc(1, 19), "b"),
    token(TOKEN_SYMBOL,  loc(1, 20), loc(1, 20), ":"),
    token(TOKEN_NAME,    loc(1, 22), loc(1, 24), "int"),
    token(TOKEN_SYMBOL,  loc(1, 25), loc(1, 25), ")"),
    token(TOKEN_SYMBOL,  loc(1, 27), loc(1, 28), "->"),
    token(TOKEN_NAME,    loc(1, 30), loc(1, 32), "int"),
    token(TOKEN_SYMBOL,  loc(1, 34), loc(1, 34), "{"),
    token(TOKEN_KEYWORD, loc(1, 36), loc(1, 41), "return"),
    token(TOKEN_NAME,    loc(1, 43), loc(1, 43), "a"),
    token(TOKEN_SYMBOL,  loc(1, 44), loc(1, 44), "+"),
    token(TOKEN_NAME,    loc(1, 45), loc(1, 45), "b"),
    token(TOKEN_SYMBOL,  loc(1, 46), loc(1, 46), ";"),
    token(TOKEN_SYMBOL,  loc(1, 48), loc(1, 48), "}"),
    token(TOKEN_EOF,     loc(1, 49), loc(1, 49), "")
  );

  in = "struct Vec2 : {\\n x: int; y: int;\\n }";
  createTest(suite, in, 14,
    token(TOKEN_KEYWORD, loc(1,  1), loc(1,  6), "struct"),
    token(TOKEN_NAME,    loc(1,  8), loc(1, 11), "Vec2"),
    token(TOKEN_SYMBOL,  loc(1, 13), loc(1, 13), ":"),
    token(TOKEN_SYMBOL,  loc(1, 15), loc(1, 15), "{"),
    token(TOKEN_NAME,    loc(2,  2), loc(2,  2), "x"),
    token(TOKEN_SYMBOL,  loc(2,  3), loc(2,  3), ":"),
    token(TOKEN_NAME,    loc(2,  5), loc(2,  7), "int"),
    token(TOKEN_SYMBOL,  loc(2,  8), loc(2,  8), ";"),
    token(TOKEN_NAME,    loc(2, 10), loc(2, 10), "y"),
    token(TOKEN_SYMBOL,  loc(2, 11), loc(2, 11), ":"),
    token(TOKEN_NAME,    loc(2, 13), loc(2, 15), "int"),
    token(TOKEN_SYMBOL,  loc(2, 16), loc(2, 16), ";"),
    token(TOKEN_SYMBOL,  loc(3,  2), loc(3,  2), "}"),
    token(TOKEN_EOF,     loc(3,  3), loc(3,  3), "")
  );

  in = "var v: int[] = [1, 2];";
  createTest(suite, in, 14,
    token(TOKEN_KEYWORD, loc(1,  1), loc(1,  3), "var"),
    token(TOKEN_NAME,    loc(1,  5), loc(1,  5), "v"),
    token(TOKEN_SYMBOL,  loc(1,  6), loc(1,  6), ":"),
    token(TOKEN_NAME,    loc(1,  8), loc(1, 10), "int"),
    token(TOKEN_SYMBOL,  loc(1, 11), loc(1, 11), "["),
    token(TOKEN_SYMBOL,  loc(1, 12), loc(1, 12), "]"),
    token(TOKEN_SYMBOL,  loc(1, 14), loc(1, 14), "="),
    token(TOKEN_SYMBOL,  loc(1, 16), loc(1, 16), "["),
    token(TOKEN_INT,     loc(1, 17), loc(1, 17), "1"),
    token(TOKEN_SYMBOL,  loc(1, 18), loc(1, 18), ","),
    token(TOKEN_INT,     loc(1, 20), loc(1, 20), "2"),
    token(TOKEN_SYMBOL,  loc(1, 21), loc(1, 21), "]"),
    token(TOKEN_SYMBOL,  loc(1, 22), loc(1, 22), ";"),
    token(TOKEN_EOF,     loc(1, 23), loc(1, 23), "")
  );
}


static void addTestsExpressions(TestSuite* suite) {
  const char* in;

  in = "(x + y) / 2";
  createTest(suite, in, 8,
    token(TOKEN_SYMBOL,  loc(1,  1), loc(1,  1), "("),
    token(TOKEN_NAME,    loc(1,  2), loc(1,  2), "x"),
    token(TOKEN_SYMBOL,  loc(1,  4), loc(1,  4), "+"),
    token(TOKEN_NAME,    loc(1,  6), loc(1,  6), "y"),
    token(TOKEN_SYMBOL,  loc(1,  7), loc(1,  7), ")"),
    token(TOKEN_SYMBOL,  loc(1,  9), loc(1,  9), "/"),
    token(TOKEN_INT,     loc(1, 11), loc(1, 11), "2"),
    token(TOKEN_EOF,     loc(1, 12), loc(1, 12), "")
  );

  in = "((x+1) * (y-2)) / 2";
  createTest(suite, in, 16,
    token(TOKEN_SYMBOL,  loc(1,  1), loc(1,  1), "("),
    token(TOKEN_SYMBOL,  loc(1,  2), loc(1,  2), "("),
    token(TOKEN_NAME,    loc(1,  3), loc(1,  3), "x"),
    token(TOKEN_SYMBOL,  loc(1,  4), loc(1,  4), "+"),
    token(TOKEN_INT,     loc(1,  5), loc(1,  5), "1"),
    token(TOKEN_SYMBOL,  loc(1,  6), loc(1,  6), ")"),
    token(TOKEN_SYMBOL,  loc(1,  8), loc(1,  8), "*"),
    token(TOKEN_SYMBOL,  loc(1, 10), loc(1, 10), "("),
    token(TOKEN_NAME,    loc(1, 11), loc(1, 11), "y"),
    token(TOKEN_SYMBOL,  loc(1, 12), loc(1, 12), "-"),
    token(TOKEN_INT,     loc(1, 13), loc(1, 13), "2"),
    token(TOKEN_SYMBOL,  loc(1, 14), loc(1, 14), ")"),
    token(TOKEN_SYMBOL,  loc(1, 15), loc(1, 15), ")"),
    token(TOKEN_SYMBOL,  loc(1, 17), loc(1, 17), "/"),
    token(TOKEN_INT,     loc(1, 19), loc(1, 19), "2"),
    token(TOKEN_EOF,     loc(1, 20), loc(1, 20), "")
  );

  in = "x & ~0b0000_1000";
  createTest(suite, in, 4,
    token(TOKEN_NAME,    loc(1,  1), loc(1,  1), "x"),
    token(TOKEN_SYMBOL,  loc(1,  3), loc(1,  3), "&"),
    token(TOKEN_SYMBOL,  loc(1,  5), loc(1,  5), "~"),
    token(TOKEN_INT,     loc(1,  6), loc(1, 16), "0b0000_1000"),
    token(TOKEN_EOF,     loc(1, 17), loc(1, 17), "")
  );

  in = "x | 0b0000_1000";
  createTest(suite, in, 4,
    token(TOKEN_NAME,    loc(1,  1), loc(1,  1), "x"),
    token(TOKEN_SYMBOL,  loc(1,  3), loc(1,  3), "|"),
    token(TOKEN_INT,     loc(1,  5), loc(1, 15), "0b0000_1000"),
    token(TOKEN_EOF,     loc(1, 16), loc(1, 16), "")
  );

  in = "x ^ 0b0000_1000";
  createTest(suite, in, 4,
    token(TOKEN_NAME,    loc(1,  1), loc(1,  1), "x"),
    token(TOKEN_SYMBOL,  loc(1,  3), loc(1,  3), "^"),
    token(TOKEN_INT,     loc(1,  5), loc(1, 15), "0b0000_1000"),
    token(TOKEN_EOF,     loc(1, 16), loc(1, 16), "")
  );

  in = "x+-*p";
  createTest(suite, in, 6,
    token(TOKEN_NAME,    loc(1,  1), loc(1,  1), "x"),
    token(TOKEN_SYMBOL,  loc(1,  2), loc(1,  2), "+"),
    token(TOKEN_SYMBOL,  loc(1,  3), loc(1,  3), "-"),
    token(TOKEN_SYMBOL,  loc(1,  4), loc(1,  4), "*"),
    token(TOKEN_NAME,    loc(1,  5), loc(1,  5), "p"),
    token(TOKEN_EOF,     loc(1,  6), loc(1,  6), "")
  );
}


static void addTestsStatements(TestSuite* suite) {
  const char* in;

  in = "if (x == 1) {\\n}";
  createTest(suite, in, 9,
    token(TOKEN_KEYWORD, loc(1,  1), loc(1,  2), "if"),
    token(TOKEN_SYMBOL,  loc(1,  4), loc(1,  4), "("),
    token(TOKEN_NAME,    loc(1,  5), loc(1,  5), "x"),
    token(TOKEN_SYMBOL,  loc(1,  7), loc(1,  8), "=="),
    token(TOKEN_INT,     loc(1, 10), loc(1, 10), "1"),
    token(TOKEN_SYMBOL,  loc(1, 11), loc(1, 11), ")"),
    token(TOKEN_SYMBOL,  loc(1, 13), loc(1, 13), "{"),
    token(TOKEN_SYMBOL,  loc(2,  1), loc(2,  1), "}"),
    token(TOKEN_EOF,     loc(2,  2), loc(2,  2), "")
  );

  in = "if (y != 0) {\\n}";
  createTest(suite, in, 9,
    token(TOKEN_KEYWORD, loc(1,  1), loc(1,  2), "if"),
    token(TOKEN_SYMBOL,  loc(1,  4), loc(1,  4), "("),
    token(TOKEN_NAME,    loc(1,  5), loc(1,  5), "y"),
    token(TOKEN_SYMBOL,  loc(1,  7), loc(1,  8), "!="),
    token(TOKEN_INT,     loc(1, 10), loc(1, 10), "0"),
    token(TOKEN_SYMBOL,  loc(1, 11), loc(1, 11), ")"),
    token(TOKEN_SYMBOL,  loc(1, 13), loc(1, 13), "{"),
    token(TOKEN_SYMBOL,  loc(2,  1), loc(2,  1), "}"),
    token(TOKEN_EOF,     loc(2,  2), loc(2,  2), "")
  );

  in = "while (!finished) {\\n}";
  createTest(suite, in, 8,
    token(TOKEN_KEYWORD, loc(1,  1), loc(1,  5), "while"),
    token(TOKEN_SYMBOL,  loc(1,  7), loc(1,  7), "("),
    token(TOKEN_SYMBOL,  loc(1,  8), loc(1,  8), "!"),
    token(TOKEN_NAME,    loc(1,  9), loc(1, 16), "finished"),
    token(TOKEN_SYMBOL,  loc(1, 17), loc(1, 17), ")"),
    token(TOKEN_SYMBOL,  loc(1, 19), loc(1, 19), "{"),
    token(TOKEN_SYMBOL,  loc(2,  1), loc(2,  1), "}"),
    token(TOKEN_EOF,     loc(2,  2), loc(2,  2), "")
  );

  in = "if (a && b || c) {\\n}";
  createTest(suite, in, 11,
    token(TOKEN_KEYWORD, loc(1,  1), loc(1,  2), "if"),
    token(TOKEN_SYMBOL,  loc(1,  4), loc(1,  4), "("),
    token(TOKEN_NAME,    loc(1,  5), loc(1,  5), "a"),
    token(TOKEN_SYMBOL,  loc(1,  7), loc(1,  8), "&&"),
    token(TOKEN_NAME,    loc(1, 10), loc(1, 10), "b"),
    token(TOKEN_SYMBOL,  loc(1, 12), loc(1, 13), "||"),
    token(TOKEN_NAME,    loc(1, 15), loc(1, 15), "c"),
    token(TOKEN_SYMBOL,  loc(1, 16), loc(1, 16), ")"),
    token(TOKEN_SYMBOL,  loc(1, 18), loc(1, 18), "{"),
    token(TOKEN_SYMBOL,  loc(2,  1), loc(2,  1), "}"),
    token(TOKEN_EOF,     loc(2,  2), loc(2,  2), "")
  );

  in = "for (i := 0; i < 10; i++) {\\n continue; }";
  createTest(suite, in, 19,
    token(TOKEN_KEYWORD, loc(1,  1), loc(1,  3), "for"),
    token(TOKEN_SYMBOL,  loc(1,  5), loc(1,  5), "("),
    token(TOKEN_NAME,    loc(1,  6), loc(1,  6), "i"),
    token(TOKEN_SYMBOL,  loc(1,  8), loc(1,  8), ":"),
    token(TOKEN_SYMBOL,  loc(1,  9), loc(1,  9), "="),
    token(TOKEN_INT,     loc(1, 11), loc(1, 11), "0"),
    token(TOKEN_SYMBOL,  loc(1, 12), loc(1, 12), ";"),
    token(TOKEN_NAME,    loc(1, 14), loc(1, 14), "i"),
    token(TOKEN_SYMBOL,  loc(1, 16), loc(1, 16), "<"),
    token(TOKEN_INT,     loc(1, 18), loc(1, 19), "10"),
    token(TOKEN_SYMBOL,  loc(1, 20), loc(1, 20), ";"),
    token(TOKEN_NAME,    loc(1, 22), loc(1, 22), "i"),
    token(TOKEN_SYMBOL,  loc(1, 23), loc(1, 23), "+"),
    token(TOKEN_SYMBOL,  loc(1, 24), loc(1, 24), "+"),
    token(TOKEN_SYMBOL,  loc(1, 25), loc(1, 25), ")"),
    token(TOKEN_SYMBOL,  loc(1, 27), loc(1, 27), "{"),
    token(TOKEN_KEYWORD, loc(2,  2), loc(2,  9), "continue"),
    token(TOKEN_SYMBOL,  loc(2, 10), loc(2, 10), ";"),
    token(TOKEN_SYMBOL,  loc(2, 12), loc(2, 12), "}"),
    token(TOKEN_EOF,     loc(2, 13), loc(2, 13), "")
  );

  in = "for (i := 9; i >= 0; i--) {\\n break; }";
  createTest(suite, in, 19,
    token(TOKEN_KEYWORD, loc(1,  1), loc(1,  3), "for"),
    token(TOKEN_SYMBOL,  loc(1,  5), loc(1,  5), "("),
    token(TOKEN_NAME,    loc(1,  6), loc(1,  6), "i"),
    token(TOKEN_SYMBOL,  loc(1,  8), loc(1,  8), ":"),
    token(TOKEN_SYMBOL,  loc(1,  9), loc(1,  9), "="),
    token(TOKEN_INT,     loc(1, 11), loc(1, 11), "9"),
    token(TOKEN_SYMBOL,  loc(1, 12), loc(1, 12), ";"),
    token(TOKEN_NAME,    loc(1, 14), loc(1, 14), "i"),
    token(TOKEN_SYMBOL,  loc(1, 16), loc(1, 17), ">="),
    token(TOKEN_INT,     loc(1, 19), loc(1, 19), "0"),
    token(TOKEN_SYMBOL,  loc(1, 20), loc(1, 20), ";"),
    token(TOKEN_NAME,    loc(1, 22), loc(1, 22), "i"),
    token(TOKEN_SYMBOL,  loc(1, 23), loc(1, 23), "-"),
    token(TOKEN_SYMBOL,  loc(1, 24), loc(1, 24), "-"),
    token(TOKEN_SYMBOL,  loc(1, 25), loc(1, 25), ")"),
    token(TOKEN_SYMBOL,  loc(1, 27), loc(1, 27), "{"),
    token(TOKEN_KEYWORD, loc(2,  2), loc(2,  6), "break"),
    token(TOKEN_SYMBOL,  loc(2,  7), loc(2,  7), ";"),
    token(TOKEN_SYMBOL,  loc(2,  9), loc(2,  9), "}"),
    token(TOKEN_EOF,     loc(2, 10), loc(2, 10), "")
  );

  in = "do {\\n print(x);\\n } while (true);";
  createTest(suite, in, 14,
    token(TOKEN_KEYWORD, loc(1,  1), loc(1,  2), "do"),
    token(TOKEN_SYMBOL,  loc(1,  4), loc(1,  4), "{"),
    token(TOKEN_NAME,    loc(2,  2), loc(2,  6), "print"),
    token(TOKEN_SYMBOL,  loc(2,  7), loc(2,  7), "("),
    token(TOKEN_NAME,    loc(2,  8), loc(2,  8), "x"),
    token(TOKEN_SYMBOL,  loc(2,  9), loc(2,  9), ")"),
    token(TOKEN_SYMBOL,  loc(2, 10), loc(2, 10), ";"),
    token(TOKEN_SYMBOL,  loc(3,  2), loc(3,  2), "}"),
    token(TOKEN_KEYWORD, loc(3,  4), loc(3,  8), "while"),
    token(TOKEN_SYMBOL,  loc(3, 10), loc(3, 10), "("),
    token(TOKEN_KEYWORD, loc(3, 11), loc(3, 14), "true"),
    token(TOKEN_SYMBOL,  loc(3, 15), loc(3, 15), ")"),
    token(TOKEN_SYMBOL,  loc(3, 16), loc(3, 16), ";"),
    token(TOKEN_EOF,     loc(3, 17), loc(3, 17), "")
  );

  in = "switch (x) {\\n case 1 -> { }\\n else -> { }\\n }";
  createTest(suite, in, 16,
    token(TOKEN_KEYWORD, loc(1,  1), loc(1,  6), "switch"),
    token(TOKEN_SYMBOL,  loc(1,  8), loc(1,  8), "("),
    token(TOKEN_NAME,    loc(1,  9), loc(1,  9), "x"),
    token(TOKEN_SYMBOL,  loc(1, 10), loc(1, 10), ")"),
    token(TOKEN_SYMBOL,  loc(1, 12), loc(1, 12), "{"),
    token(TOKEN_KEYWORD, loc(2,  2), loc(2,  5), "case"),
    token(TOKEN_INT,     loc(2,  7), loc(2,  7), "1"),
    token(TOKEN_SYMBOL,  loc(2,  9), loc(2, 10), "->"),
    token(TOKEN_SYMBOL,  loc(2, 12), loc(2, 12), "{"),
    token(TOKEN_SYMBOL,  loc(2, 14), loc(2, 14), "}"),
    token(TOKEN_KEYWORD, loc(3,  2), loc(3,  5), "else"),
    token(TOKEN_SYMBOL,  loc(3,  7), loc(3,  8), "->"),
    token(TOKEN_SYMBOL,  loc(3, 10), loc(3, 10), "{"),
    token(TOKEN_SYMBOL,  loc(3, 12), loc(3, 12), "}"),
    token(TOKEN_SYMBOL,  loc(4,  2), loc(4,  2), "}"),
    token(TOKEN_EOF,     loc(4,  3), loc(4,  3), "")
  );
}


TestResult lexer_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<lexer>", "Test lexer.");
  addTest(&suite, &testCreation,  "testCreation");
  addTest(&suite, &testErrorMsgs, "testErrorMsgs");
  addTestsEndOfLine(&suite);
  addTestsTokenName(&suite);
  addTestsTokenInt(&suite);
  addTestsTokenHexInt(&suite);
  addTestsTokenBinInt(&suite);
  addTestsWhitespaces(&suite);
  addTestsTokenKeyword(&suite);
  addTestsTokenSeparator(&suite);
  addTestsTokenOperator(&suite);
  addTestsTokenComment(&suite);
  addTestsDeclarations(&suite);
  addTestsExpressions(&suite);
  addTestsStatements(&suite);
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
