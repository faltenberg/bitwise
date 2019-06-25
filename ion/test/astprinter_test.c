#include "cunit.h"
#include "util.h"

#include "astprinter.h"

#include "parser.h"
#include "strintern.h"


#define createTest(in, exp) __createTest(__FILE__, __LINE__, in, exp)
static bool __createTest(const char* file, int line, const char* input, const char* exp) {
  TestResult result = {};

  {
    Source src = sourceFromString(input);
    ASTNode* node = parse(&src);
    string s = printAST(node);
    TEST(__assertEqualStr(file, line, s, exp));
//    printf("%.*s\n", s.len, s.chars);
//    printf("%s\n", exp);
    strFree(&s);
    deleteNode(node);
    deleteSource(&src);
  }

  return result.failedTests == 0;
}


static TestResult testPrintEmptyString() {
  TestResult result = {};
  TEST(createTest("", "(none)"));
  TEST(createTest(" ", "(none)"));
  TEST(createTest("\n", "(none)"));
  return result;
}


static TestResult testPrintError() {
  TestResult result = {};
  TEST(createTest("$",
       "(error \"<cstring>:1:1: \e[31mError:\e[39m illegal character '$'\" in (none))"));
  TEST(createTest("if",
       "(error \"<cstring>:1:1: \e[31mError:\e[39m unexpected token TOKEN_KEYWORD\" in (none))"));
  TEST(createTest("_",
       "(error \"<cstring>:1:1: \e[31mError:\e[39m unexpected token TOKEN_KEYWORD\" in (none))"));
  TEST(createTest("// comment",
       "(error \"<cstring>:1:1: \e[31mError:\e[39m unexpected token TOKEN_COMMENT\" in (none))"));
  return result;
}


static TestResult testPrintExprName() {
  TestResult result = {};
  TEST(createTest("x", "x"));
  TEST(createTest("_x1", "_x1"));
  return result;
}


static TestResult testPrintExprInt() {
  TestResult result = {};
  TEST(createTest("123", "123"));
  TEST(createTest("1x",
       "(error \"<cstring>:1:2: \e[31mError:\e[39m invalid integer format\" in (none))"));
  return result;
}



static TestResult testPrintExprUnop() {
  TestResult result = {};
  TEST(createTest("+x",  "(+ x)"));
  TEST(createTest("-x",  "(- x)"));
  TEST(createTest("!x",  "(! x)"));
  TEST(createTest("~x",  "(~ x)"));
  TEST(createTest("/x",
       "(error \"<cstring>:1:1: \e[31mError:\e[39m invalid unary operator \"/\"\" in (none))"));
  TEST(createTest("+",
       "(+ (error \"<cstring>:1:2: \e[31mError:\e[39m unexpected token TOKEN_EOF\" in (none)))"));
  TEST(createTest("!if",
       "(! (error \"<cstring>:1:2: \e[31mError:\e[39m unexpected token TOKEN_KEYWORD\" in (none)))"));
  TEST(createTest("!#x",
       "(! (error \"<cstring>:1:2: \e[31mError:\e[39m illegal character '#'\" in (none)))"));
  TEST(createTest("!!x", "(! (! x))"));
  return result;
}

/*
static TestResult testPrintExprBinop() {
  TestResult result = {};
  TEST(createTest("x+y", "(+ x y)"));
  TEST(createTest("x-y", "(- x y)"));
  TEST(createTest("x*y", "(* x y)"));
  TEST(createTest("x/y", "(/ x y)"));
  TEST(createTest("x%y", "(% x y)"));
  TEST(createTest("x+y+z", "(+ (+ x y) z)"));
  return result;
}


static TestResult testPrintExprParen() {
  TestResult result = {};
  TEST(createTest("(x)", "(x)"));
  TEST(createTest("((x))", "((x))"));
  return result;
}
*/


TestResult astprinter_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<astprinter>", "Test AST printer.");
  addTest(&suite, testPrintEmptyString);
  addTest(&suite, testPrintError);
  addTest(&suite, testPrintExprName);
  addTest(&suite, testPrintExprInt);
  addTest(&suite, testPrintExprUnop);
//  addTest(&suite, testPrintExprBinop);
//  addTest(&suite, testPrintExprParen);
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  strinternFree();
  return result;
}
