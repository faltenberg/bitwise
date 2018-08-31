#include "cunit.h"


/**
 * Example
 * -------
 *
 * ```c
 * #include "cunit.h"
 *
 * extern TestResult template_alltests();
 *
 * int main() {
 *   TestResult result = {};
 *   result = unite(result, template_alltests());
 *   // printf("Result: %d of %d tests failed\n", result.failedTests, result.totalTests);
 * }
 * ```
 *
 * Expand
 * ------
 *
 * 1. change filename if desired
 * 2. for convinience and to avoid name collisions each *testXYZ.c* file
 *    shall have a `testXYZ_alltests()` function
 * 3. write test cases and don't forget to add them inside `testXYZ_alltests()`
 *    (use macros for more expressibility)
 * 4. write a `main()` function
 */


static TestResult simpleTest() {
  TestResult result = {};
  apply(&result, assertEqualInt(13, 42));
  return result;
}


static TestResult testWithMacros() {
  TestResult result = {};         // NECESSARY, macros expect it
  INFO("test with macros");       // simply prints a message
  TEST(assertEqualInt(13, 42));   // result += assertEqualInt(13, 42);
  SKIP(assertEqualInt(42, 42));   // result = result
  FAIL("intended to fail");       // result += false
  ABORT(assertEqualInt(42, 42));  // like TEST(assertEqualInt(42, 42)) BUT
  ABORT(assertEqualInt(13, 42));  // will exit testWithMacros() if assertion fails
  TEST(assertEqualInt(42, 42));   // won't be reached
  return result;                  // result has 3 failed tests out of 4
}


TestResult template_alltests() {
  TestSuite suite = newSuite(__FILE__, "Test Suite Template");
  addTest(&suite, &simpleTest,     "simpleTest");
  addTest(&suite, &testWithMacros, "testWithMacros");
  TestResult result = run(&suite);
  deleteSuite(&suite);
  return result;
}
