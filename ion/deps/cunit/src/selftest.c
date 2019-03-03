#include "cunit.h"

#include <float.h>


static TestResult simpleTest() {
  TestResult result = {};
  apply(&result, assertEqualInt(42, 42));
  return result;
}


static TestResult testMacros() {
  TestResult result = {};
  INFO("INFO(\"test macros\")");
  INFO("test macros");
  INFO("TEST(assertEqualInt(42, 42));");
  TEST(assertEqualInt(42, 42));
  INFO("TEST(assertEqualInt(13, 42));");
  TEST(assertEqualInt(13, 42));
  INFO("SKIP(assertEqualInt(13, 42));");
  SKIP(assertEqualInt(13, 42));
  INFO("FAIL(\"intended to fail\");");
  FAIL("intended to fail");
  INFO("ABORT(assertEqualInt(42, 42));");
  ABORT(assertEqualInt(42, 42));
  INFO("ABORT(assertEqualInt(13, 42));");
  ABORT(assertEqualInt(13, 42));
  FAIL("shouldn't be executed at all due to prior ABORT()");
  return result;
}


static TestResult testAssertions() {
  TestResult result = {};

  INFO("assertFalse()");
  TEST(assertFalse(42 == 42));
  INFO("assertTrue()");
  TEST(assertTrue(13 == 42));

  INFO("assertEqual<bool>()");
  TEST(assertEqualBool(false, true));
  INFO("assertNotEqual<bool>()");
  TEST(assertNotEqualBool(false, false));

  INFO("assertEqual<int>()");
  TEST(assertEqualInt(13, 42));
  INFO("assertNotEqual<int>()");
  TEST(assertNotEqualInt(42, 42));

  INFO("assertEqual<char>()");
  TEST(assertEqualChar('B', 'A'));
  INFO("assertNotEqual<char>()");
  TEST(assertNotEqualChar('A', 'A'));

  INFO("assertEqual<size_t>()");
  TEST(assertEqualSize(sizeof(result), 0));
  INFO("assertNotEqual<size_t>()");
  TEST(assertNotEqualSize(sizeof(result), sizeof(result)));

  INFO("assertEqual<float>()");
  TEST(assertEqualFloat(0.0f, 1.0f));
  INFO("assertNotEqual<float>()");
  TEST(assertNotEqualFloat(1.0f, 1.0f));

  INFO("assertEqual<double>()");
  TEST(assertEqualDouble(0.0, 1.0));
  INFO("assertNotEqual<double>()");
  TEST(assertNotEqualDouble(1.0, 1.0));

  INFO("assertEqual<string>()");
  TEST(assertEqualString("abcXef", "abcdef"));
  INFO("assertNotEqual<string>()");
  TEST(assertNotEqualString("abc", "abc"));

  INFO("assertNull()");
  TEST(assertNull(&result));
  INFO("assertNotNull()");
  TEST(assertNotNull(NULL));

  INFO("assertSame()");
  TEST(assertSame(&result, &result + 1));
  INFO("assertNotSame()");
  TEST(assertNotSame(&result, &result));

  {
  int val = 0x1122FF44;
  unsigned char exp[] = {0x44, 0x33, 0x22, 0x11};
  INFO("assertEqualMemory()");
  TEST(assertEqualMemory(&val, exp, 4));
  }

  return result;
}


static TestResult testCmpFloat() {
  TestResult result = {};

  TEST(assertEqualFloat(1.0f, 1.0f));
  TEST(assertEqualFloat(-0.0f, 0.0f));
  TEST(assertEqualFloat(1.0f/0.0f, 1.0f/0.0f));
  TEST(assertEqualFloat(-1.0f/0.0f, -1.0f/0.0f));
  TEST(assertEqualFloat(1.0f+FLT_MIN, 1.0f));
  TEST(assertEqualFloat(1.0f+FLT_EPSILON, 1.0f));
  TEST(assertEqualFloat(FLT_MAX+FLT_EPSILON, FLT_MAX));
  TEST(assertEqualFloat(FLT_MAX+FLT_MIN, FLT_MAX));
  TEST(assertEqualFloat(FLT_MAX+1.0f, FLT_MAX));
  TEST(assertEqualFloat(FLT_MAX+FLT_MAX, 1.0f/0.0f));

  TEST(assertNotEqualFloat(0.0f/0.0f, 0.0f/0.0f));
  TEST(assertNotEqualFloat(1.0f/0.0f, FLT_MAX));
  TEST(assertNotEqualFloat(0.0f, FLT_MIN));

  return result;
}


static TestResult testCmpString() {
  TestResult result = {};

  TEST(assertEqualString("", ""));
  TEST(assertNotEqualString("", "a"));
  TEST(assertNotEqualString("a", ""));
  TEST(assertEqualString("a", "a"));
  TEST(assertEqualString("abcdef", "abcdef"));
  TEST(assertNotEqualString("a", "ab"));
  TEST(assertNotEqualString("ab", "a"));
  TEST(assertNotEqualString("ab", "\t"));

  return result;
}

static TestResult testCmpMemory() {
  TestResult result = {};

  {
  int val = 0x11223344;
  unsigned char exp[] = {0x44, 0x33, 0x22, 0x11};
  ABORT(assertEqualSize(sizeof(exp), 4));
  ABORT(assertEqualSize(sizeof(val), sizeof(exp)));
  }

  {
  int val = 0x11223344;
  unsigned char exp[] = {0x44, 0x33, 0x22, 0x11};
  TEST(assertEqualMemory(&val, exp, 4));
  }

  {
  int val = 0xAABBCCDD;
  unsigned char exp[] = {0x44, 0x33, 0x22, 0x11};
  TEST(assertEqualMemory(&val, exp, 0));
  }

  {
  int val = 0xFF223344;
  unsigned char exp[] = {0x44, 0x33, 0x22, 0x11};
  TEST(assertEqualMemory(&val, exp, 1));
  }

  {
  int val = 0x11FF3344;
  unsigned char exp[] = {0x44, 0x33, 0x22, 0x11};
  INFO("next test intended to fail");
  TEST(assertEqualMemory(&val, exp, 4));
  }

  return result;
}


TestResult selftest_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite(__FILE__, "Self Test and Demo");
  addTest(&suite, &simpleTest,     "simpleTest");
  addTest(&suite, &testMacros,     "testMacros");
  addTest(&suite, &testAssertions, "testAssertions");
  addTest(&suite, &testCmpFloat,   "testCmpFloat");
  addTest(&suite, &testCmpString,  "testCmpString");
  addTest(&suite, &testCmpMemory,  "testCmpMemory");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
