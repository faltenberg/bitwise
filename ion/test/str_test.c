#include "cunit.h"

#include "str.h"


static TestResult testStringCreation() {
  TestResult result = {};

  {
    string s = fromCString("");
    TEST(assertSame(s.chars, ""));
    TEST(assertEqualSize(s.len, 0));
  }

  {
    string s = fromCString("foo");
    TEST(assertSame(s.chars, "foo"));
    TEST(assertEqualSize(s.len, 3));
  }

  {
    string s = fromRange("foo", "foo"+3);
    TEST(assertSame(s.chars, "foo"));
    TEST(assertEqualSize(s.len, 3));
  }

  {
    string s = fromRange("foo", "foo"+2);
    TEST(assertSame(s.chars, "foo"));
    TEST(assertEqualSize(s.len, 2));
  }

  {
    string s = fromRange("foo", "foo"+0);
    TEST(assertSame(s.chars, "foo"));
    TEST(assertEqualSize(s.len, 0));
  }

  {
    string s = fromRange("foo"+1, "foo"+0);
    TEST(assertSame(s.chars, "foo"+1));
    TEST(assertEqualSize(s.len, 0));
  }

  return result;
}


static TestResult testStringComparison() {
  TestResult result = {};

  {
    string s = fromCString("foo");
    TEST(assertTrue(streq(s, "foo")));
    TEST(assertFalse(streq(s, "fo")));
    TEST(assertFalse(streq(s, "bar")));
  }

  {
    string s = fromCString("foo");
    TEST(assertTrue(strequal(s, fromCString("foo"))));
    TEST(assertFalse(strequal(s, fromCString("fo"))));
  }

  {
    string s = fromRange("foo"+1, "foo"+1);
    TEST(assertTrue(streq(s, "")));
  }

  {
    string s = fromRange("foo"+1, "foo"+0);
    TEST(assertTrue(streq(s, "")));
  }

  {
    const char* lorem = "lorem ipsum dolor";
    string s = fromRange(lorem+6, lorem+11);
    TEST(assertEqualSize(s.len, 5));
    TEST(assertTrue(streq(s, "ipsum")));
    TEST(assertTrue(strequal(s, fromCString("ipsum"))));
  }

  return result;
}


TestResult str_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<str>", "Test strings.");
  addTest(&suite, &testStringCreation,   "testStringCreation");
  addTest(&suite, &testStringComparison, "testStringComparison");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
