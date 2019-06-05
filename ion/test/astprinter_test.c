#include "cunit.h"
#include "util.h"

#include "astprinter.h"

#include "parser.h"


#define createTest(in, exp)  __test(__FILE__, __LINE__, in, exp)
static bool __test(const char* file, int line, const char* input, const char* exp) {
  TestResult result = {};

  {
    Source src = sourceFromString(input);
    ASTNode* node = parse(&src);
    string s = printAST(node);
    TEST(__assertEqualStr(file, line, s, exp));
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
  TEST(createTest("(", "(error <cstring>:1:2: \e[31mError:\e[39m missing closing ')')"));
  return result;
}


static TestResult testPrintExprInt() {
  TestResult result = {};
  TEST(createTest("123", "123"));
  return result;
}


static TestResult testPrintExprName() {
  TestResult result = {};
  TEST(createTest("x", "x"));
  TEST(createTest("_x1", "_x1"));
  return result;
}


static TestResult testPrintExprUnop() {
  TestResult result = {};
  TEST(createTest("-x", "(- x)"));
  TEST(createTest("+x", "(+ x)"));
  TEST(createTest("!x", "(! x)"));
  TEST(createTest("~x", "(~ x)"));
  return result;
}


TestResult astprinter_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<astprinter>", "Test AST printer.");
  addTest(&suite, &testPrintEmptyString, "testPrintEmptyString");
  addTest(&suite, &testPrintError,       "testPrintError");
  addTest(&suite, &testPrintExprInt,     "testPrintExprInt");
  addTest(&suite, &testPrintExprName,    "testPrintExprName");
  addTest(&suite, &testPrintExprUnop,    "testPrintExprUnop");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
