#include "cunit.h"
#include "util.h"

#include "error.h"


static TestResult testCreation() {
  TestResult result = {};

  {
    Error error = createError(loc(1, 1), stringFromArray("error message"), NULL);
    TEST(assertEqualInt(error.location.line, 1));
    TEST(assertEqualInt(error.location.pos, 1));
    TEST(assertEqualStr(error.message, "error message"));
    TEST(assertNull(error.cause));
  }

  {
    Error cause = createError(loc(1, 1), stringFromArray("original cause"), NULL);
    Error error = createError(loc(2, 1), stringFromArray("error message"), &cause);
    TEST(assertSame(error.cause, &cause));
    TEST(assertEqualInt(error.cause->location.line, 1));
    TEST(assertEqualInt(error.cause->location.pos, 1));
    TEST(assertEqualStr(error.cause->message, "original cause"));
  }

  {
    Source src = sourceFromString("  123xyz  // error");
    Error error = generateError(loc(1, 6), &src, loc(1, 3), loc(1, 8),
                                "invalid integer character '%c'", 'x');
    TEST(assertEqualInt(error.location.line, 1));
    TEST(assertEqualInt(error.location.pos, 6));
    TEST(assertEqualStr(error.message,
         "<cstring>:1:6: \e[31merror:\e[39m invalid integer character 'x'\n"
         "  123xyz  // error\n  \e[32m~~~^~~\e[39m\n"));
    TEST(assertNull(error.cause));
    deleteSource(&src);
    strFree(&error.message);
  }

  {
    Source src = sourceFromString("  $23xyz  // error");
    Error error = generateError(loc(1, 3), &src, loc(1, 3), loc(1, 3),
                                "invalid character '%c'", '$');
    TEST(assertEqualInt(error.location.line, 1));
    TEST(assertEqualInt(error.location.pos, 3));
    TEST(assertEqualStr(error.message,
         "<cstring>:1:3: \e[31merror:\e[39m invalid character '$'\n"
         "  $23xyz  // error\n  \e[32m^\e[39m\n"));
    TEST(assertNull(error.cause));
    deleteSource(&src);
    strFree(&error.message);
  }

  {
    Source src = sourceFromString("  $23xyz\n");
    Error error = generateError(loc(1, 3), &src, loc(1, 3), loc(1, 3),
                                "invalid character '%c'", '$');
    TEST(assertEqualInt(error.location.line, 1));
    TEST(assertEqualInt(error.location.pos, 3));
    TEST(assertEqualStr(error.message,
         "<cstring>:1:3: \e[31merror:\e[39m invalid character '$'\n"
         "  $23xyz\n  \e[32m^\e[39m\n"));
    TEST(assertNull(error.cause));
    deleteSource(&src);
    strFree(&error.message);
  }

  return result;
}


static TestResult testDeletion() {
  TestResult result = {};

  {
    Error error = createError(loc(1, 1), stringFromArray("error message"), NULL);
    deleteError(&error);
    TEST(assertEqualInt(error.location.line, 0));
    TEST(assertEqualInt(error.location.pos, 0));
    TEST(assertEqualStr(error.message, ""));
    TEST(assertNull(error.cause));
  }

  {
    Source src = sourceFromString("  123xyz  // error");
    Error cause = generateError(loc(1, 6), &src, loc(1, 3), loc(1, 8),
                                "invalid integer character '%c'", 'x');
    Error error = createError(loc(2, 1), stringFromArray("error message"), &cause);
    deleteError(&error);
    TEST(assertEqualInt(cause.location.line, 0));
    TEST(assertEqualInt(cause.location.pos, 0));
    TEST(assertEqualStr(cause.message, ""));
    TEST(assertNull(cause.cause));
    TEST(assertEqualInt(error.location.line, 0));
    TEST(assertEqualInt(error.location.pos, 0));
    TEST(assertEqualStr(error.message, ""));
    TEST(assertNull(error.cause));
    deleteSource(&src);
  }

  return result;
}


TestResult error_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<error>", "Test errors.");
  addTest(&suite, &testCreation, "testCreation");
  addTest(&suite, &testDeletion, "testDeletion");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
