#include "cunit/cunit.h"
#include "strintern.h"


static TestResult testIdentity() {
  TestResult result = {};
  const char* a = "abc";
  const char b[] = {'a', 'b', 'c'};
  ABORT(assertNotSame(a, b));
  TEST(assertSame(strintern(a), strintern(b)));
  return result;
}


static TestResult testEmptyString() {
  TestResult result = {};
  const char* a = "";
  const char b[] = {};
  ABORT(assertNotSame(a, b));
  TEST(assertSame(strintern(a), strintern(b)));
  return result;
}


static TestResult testStoredData() {
  TestResult result = {};
  TEST(assertEqualString(strintern("abc"), "abc"));
  return result;
}


static TestResult testPrefix() {
  TestResult result = {};
  const char* a = "abcd";
  const char b[] = {'a', 'b', 'c'};
  ABORT(assertNotSame(a, b));
  TEST(assertNotSame(strintern(a), strintern(b)));
  return result;
}


TestResult strintern_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<strintern>", "Test string interning.");
  addTest(&suite, &testIdentity,    "testIdentity");
  addTest(&suite, &testEmptyString, "testEmptyString");
  addTest(&suite, &testStoredData,  "testStoredData");
  addTest(&suite, &testPrefix,      "testPrefix");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
