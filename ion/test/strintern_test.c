#include "cunit.h"

#include "strintern.h"


static TestResult testIdentity() {
  TestResult result = {};
  const char* a = "abc";
  const char b[] = {'a', 'b', 'c', '\0'};
  ABORT(assertNotSame(a, b));
  TEST(assertSame(strintern(a), strintern(b)));
  return result;
}


static TestResult testEmptyString() {
  TestResult result = {};
  const char* a = "";
  const char b[] = {'\0'};
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
  const char* a = "1234";
  const char b[] = {'1', '2', '3', '\0'};
  ABORT(assertNotSame(a, b));
  TEST(assertNotSame(strintern(a), strintern(b)));
  return result;
}


static TestResult testFree() {
  TestResult result = {};
  const char* a = strintern("abc");
  strinternFree();
  const char* b = strintern("abc");
  TEST(assertNotSame(a, b));
  return result;
}


TestResult strintern_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<strintern>", "Test string interning.");
  addTest(&suite, &testIdentity,    "testIdentity");
  addTest(&suite, &testEmptyString, "testEmptyString");
  addTest(&suite, &testStoredData,  "testStoredData");
  addTest(&suite, &testPrefix,      "testPrefix");
  addTest(&suite, &testFree,        "testFree");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  strinternFree();
  return result;
}
