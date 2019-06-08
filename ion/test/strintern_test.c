#include "cunit.h"
#include "util.h"

#include "strintern.h"


static TestResult testEmptyString() {
  TestResult result = {};

  {
    string s = strintern("");
    TEST(assertNotNull(s.chars));
    TEST(assertNotSame(s.chars, ""));
    TEST(assertEqualSize(s.len, 0));
    strinternFree();
  }

  return result;
}


static TestResult testFirstInterning() {
  TestResult result = {};

  {
    string s = strintern("abc");
    TEST(assertNotNull(s.chars));
    TEST(assertNotSame(s.chars, "abc"));
    TEST(assertEqualSize(s.len, 3));
    TEST(assertEqualStr(s, "abc"));
    strinternFree();
  }

  return result;
}


static TestResult testConsecutiveInterning() {
  TestResult result = {};

  {
    string a = strintern("abc");
    string b = strintern("abc");
    TEST(assertSame(a.chars, b.chars));
    TEST(assertEqualSize(a.len, b.len));
    strinternFree();
  }

  {
    string a = strintern("abc");
    string b = strintern("abc_");
    TEST(assertNotSame(a.chars, b.chars));
    TEST(assertNotEqualSize(a.len, b.len));
    strinternFree();
  }

  {
    string a = strintern("");
    string b = strintern("");
    TEST(assertSame(a.chars, b.chars));
    TEST(assertEqualSize(a.len, b.len));
    strinternFree();
  }

  return result;
}


static TestResult testSubstringInterning() {
  TestResult result = {};

  {
    string a = strintern("abc");
    string b = strintern("ab");
    TEST(assertSame(a.chars, b.chars));
    TEST(assertNotEqualSize(a.len, b.len));
    TEST(assertEqualSize(b.len, 2));
    TEST(assertEqualStr(b, "ab"));
    strinternFree();
  }

  {
    string a = strintern("abc");
    string b = strintern("bc");
    TEST(assertSame(a.chars+1, b.chars));
    TEST(assertNotEqualSize(a.len, b.len));
    TEST(assertEqualSize(b.len, 2));
    TEST(assertEqualStr(b, "bc"));
    strinternFree();
  }

  {
    string a = strintern("abc");
    string b = strintern("");
    TEST(assertSame(a.chars, b.chars));
    TEST(assertNotEqualSize(a.len, b.len));
    TEST(assertEqualSize(b.len, 0));
    strinternFree();
  }

  {
    string a = strintern("abc");
    string b = strintern("abcdef");
    string c = strintern("bc");
    TEST(assertNotSame(a.chars, b.chars));
    TEST(assertSame(c.chars, a.chars+1));
    TEST(assertEqualSize(c.len, 2));
    TEST(assertEqualStr(c, "bc"));
    strinternFree();
  }

  {
    string a = strintern("abc");
    string b = strintern("abcdef");
    string c = strintern("bcd");
    TEST(assertNotSame(a.chars, b.chars));
    TEST(assertSame(c.chars, b.chars+1));
    TEST(assertEqualSize(c.len, 3));
    TEST(assertEqualStr(c, "bcd"));
    strinternFree();
  }

  {
    string s = strintern("foobar");
    string f = strintern("foo");
    string b = strintern("bar");
    TEST(assertSame(f.chars, s.chars));
    TEST(assertSame(b.chars, s.chars+3));
    TEST(assertEqualSize(f.len, 3));
    TEST(assertEqualSize(b.len, 3));
    TEST(assertEqualStr(f, "foo"));
    TEST(assertEqualStr(b, "bar"));
  }

  return result;
}


static TestResult testRangeInterning() {
  TestResult result = {};

  {
    string s = strinternRange(&"abc"[0], &"abc"[3]);
    TEST(assertNotSame(s.chars, "abc"));
    TEST(assertEqualSize(s.len, 3));
    TEST(assertEqualStr(s, "abc"));
    strinternFree();
  }

  {
    string s = strinternRange(&"abc"[0], &"abc"[2]);
    TEST(assertNotSame(s.chars, "abc"));
    TEST(assertEqualSize(s.len, 2));
    TEST(assertEqualStr(s, "ab"));
    strinternFree();
  }

  {
    string s = strinternRange(&"abc"[0], &"abc"[0]);
    TEST(assertNotNull(s.chars));
    TEST(assertEqualSize(s.len, 0));
    TEST(assertEqualStr(s, ""));
    strinternFree();
  }

  {
    string s = strinternRange(&"abc"[1], &"abc"[0]);
    TEST(assertNotNull(s.chars));
    TEST(assertEqualSize(s.len, 0));
    TEST(assertEqualStr(s, ""));
    strinternFree();
  }

  {
    string a = strintern("abc");
    string b = strinternRange(a.chars+0, a.chars+3);
    TEST(assertSame(b.chars, a.chars));
    TEST(assertEqualSize(b.len, 3));
    TEST(assertEqualStr(b, "abc"));
    strinternFree();
  }

  {
    string a = strintern("abc");
    string b = strinternRange(a.chars+1, a.chars+3);
    TEST(assertSame(b.chars, a.chars+1));
    TEST(assertEqualSize(b.len, 2));
    TEST(assertEqualStr(b, "bc"));
    strinternFree();
  }

  {
    string a = strintern("abc");
    string b = strinternRange(a.chars+1, a.chars+2);
    TEST(assertSame(b.chars, a.chars+1));
    TEST(assertEqualSize(b.len, 1));
    TEST(assertEqualStr(b, "b"));
    strinternFree();
  }

  {
    string a = strintern("abc");
    string b = strintern("abcdef");
    string c = strinternRange(b.chars+1, b.chars+3);
    TEST(assertSame(c.chars, a.chars+1));
    TEST(assertEqualSize(c.len, 2));
    TEST(assertEqualStr(c, "bc"));
    strinternFree();
  }

  return result;
}


TestResult strintern_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<strintern>", "Test string interning.");
  addTest(&suite, &testEmptyString,          "testEmptyString");
  addTest(&suite, &testFirstInterning,       "testFirstInterning");
  addTest(&suite, &testConsecutiveInterning, "testConsecutiveInterning");
  addTest(&suite, &testSubstringInterning,   "testSubstringInterning");
  addTest(&suite, &testRangeInterning,       "testRangeInterning");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
