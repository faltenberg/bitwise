#include "cunit.h"

#include "str.h"

#include <stdlib.h>


static TestResult testStringCreation() {
  TestResult result = {};

  {
    string s = stringFromArray("");
    TEST(assertSame(s.chars, ""));
    TEST(assertEqualSize(s.len, 0));
    TEST(assertFalse(s.owned));
  }

  {
    string s = stringFromArray("foo");
    TEST(assertSame(s.chars, "foo"));
    TEST(assertEqualSize(s.len, 3));
    TEST(assertFalse(s.owned));
  }

  {
    string s = stringFromRange("foo", &"foo"[3]);
    TEST(assertSame(s.chars, "foo"));
    TEST(assertEqualSize(s.len, 3));
    TEST(assertFalse(s.owned));
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

  {
    string s = stringFromPrint("s: %d", 42);
    TEST(assertEqualSize(s.len, 5));
    TEST(assertEqualString(s.chars, "s: 42"));
    ABORT(assertTrue(s.owned));
    strFree(&s);
  }

  {
    string s = stringFromPrint("a longer text: %d", 42);
    TEST(assertEqualSize(s.len, 17));
    TEST(assertEqualString(s.chars, "a longer text: 42"));
    ABORT(assertTrue(s.owned));
    strFree(&s);
  }

  return result;
}


static TestResult testStringDeletion() {
  TestResult result = {};

  {
    string s = stringFromPrint("s: %d", 42);
    strFree(&s);
    TEST(assertSame(s.chars, ""));
    TEST(assertEqualInt(s.len, 0));
  }

  {
    string s = stringFromArray("foo");
    strFree(&s);
    TEST(assertSame(s.chars, ""));
    TEST(assertEqualInt(s.len, 0));
  }

  {
    string s = stringFromRange("foo", &"foo"[3]);
    strFree(&s);
    TEST(assertSame(s.chars, ""));
    TEST(assertEqualInt(s.len, 0));
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
  addTest(&suite, testStringCreation);
  addTest(&suite, testStringDeletion);
  addTest(&suite, testStringComparison);
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
