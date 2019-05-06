#include "cunit.h"

#include "str.h"


static TestResult testStringCreation() {
  TestResult result = {};

  {
    string s = stringFromArray("");
    TEST(assertSame(s.chars, ""));
    TEST(assertEqualSize(s.len, 0));
  }

  {
    string s = stringFromArray("foo");
    TEST(assertSame(s.chars, "foo"));
    TEST(assertEqualSize(s.len, 3));
  }

  {
    string s = stringFromRange("foo", &"foo"[3]);
    TEST(assertSame(s.chars, "foo"));
    TEST(assertEqualSize(s.len, 3));
  }

  {
    string s = stringFromRange("foo", &"foo"[2]);
    TEST(assertSame(s.chars, "foo"));
    TEST(assertEqualSize(s.len, 2));
  }

  {
    string s = stringFromRange("foo", &"foo"[0]);
    TEST(assertSame(s.chars, "foo"));
    TEST(assertEqualSize(s.len, 0));
  }

  {
    string s = stringFromRange(&"foo"[1], &"foo"[0]);
    TEST(assertSame(s.chars, &"foo"[1]));
    TEST(assertEqualSize(s.len, 0));
  }

  return result;
}


static TestResult testStringComparison() {
  TestResult result = {};

  {
    string s = stringFromArray("foo");
    TEST(assertTrue(cstrequal(s, "foo")));
    TEST(assertFalse(cstrequal(s, "fo")));
    TEST(assertFalse(cstrequal(s, "bar")));
  }

  {
    string s = stringFromArray("foo");
    TEST(assertTrue(strequal(s, stringFromArray("foo"))));
    TEST(assertFalse(strequal(s, stringFromArray("bar"))));
  }

  {
    string s = stringFromRange("foo", &"foo"[1]);
    TEST(assertTrue(strequal(s, stringFromArray("f"))));
    TEST(assertFalse(strequal(s, stringFromArray("fo"))));
  }

  {
    string s = stringFromRange(&"foo"[1], &"foo"[1]);
    TEST(assertTrue(cstrequal(s, "")));
    TEST(assertFalse(cstrequal(s, "f")));
    TEST(assertFalse(cstrequal(s, "o")));
  }

  {
    string s = stringFromRange(&"foo"[1], &"foo"[0]);
    TEST(assertTrue(cstrequal(s, "")));
    TEST(assertFalse(cstrequal(s, "f")));
    TEST(assertFalse(cstrequal(s, "o")));
  }

  {
    const char* lorem = "lorem ipsum dolor";
    string s = stringFromRange(lorem+6, lorem+11);
    TEST(assertEqualSize(s.len, 5));
    TEST(assertTrue(cstrequal(s, "ipsum")));
    TEST(assertTrue(strequal(s, stringFromArray("ipsum"))));
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
