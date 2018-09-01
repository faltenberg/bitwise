#include "cunit/cunit.h"


static TestResult simpleTest() {
  TestResult result = {};
  apply(&result, assertEqualInt(13, 42));
  return result;
}


TestResult lexer_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<lexer>", "Test lexer for ION grammar.");
  addTest(&suite, &simpleTest, "simpleTest");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
