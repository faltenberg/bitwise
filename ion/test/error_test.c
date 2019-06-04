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
    string msg = generateError(&src, loc(1, 3), loc(1, 6), loc(1, 8),
                               "invalid integer character '%c'", 'x');
    Error error = createError(loc(1, 6), msg, NULL);
    TEST(assertEqualInt(error.location.line, 1));
    TEST(assertEqualInt(error.location.pos, 6));
    TEST(assertEqualStr(error.message,
         "<cstring>:1:6: \e[31mError:\e[39m invalid integer character 'x'\n"
         "  123xyz  // error\n  \e[32m~~~^~~\e[39m\n"));
    TEST(assertNull(error.cause));
    deleteSource(&src);
    strFree(&msg);
  }

  {
    Source src = sourceFromString("  $23xyz  // error");
    string msg = generateError(&src, loc(1, 3), loc(1, 3), loc(1, 3),
                               "invalid character '%c'", '$');
    Error error = createError(loc(1, 3), msg, NULL);
    TEST(assertEqualInt(error.location.line, 1));
    TEST(assertEqualInt(error.location.pos, 3));
    TEST(assertEqualStr(error.message,
         "<cstring>:1:3: \e[31mError:\e[39m invalid character '$'\n"
         "  $23xyz  // error\n  \e[32m^\e[39m\n"));
    TEST(assertNull(error.cause));
    deleteSource(&src);
    strFree(&msg);
  }

  {
    Source src = sourceFromString("  $23xyz\n");
    string msg = generateError(&src, loc(1, 3), loc(1, 3), loc(1, 3),
                               "invalid character '%c'", '$');
    Error error = createError(loc(1, 3), msg, NULL);
    TEST(assertEqualInt(error.location.line, 1));
    TEST(assertEqualInt(error.location.pos, 3));
    TEST(assertEqualStr(error.message,
         "<cstring>:1:3: \e[31mError:\e[39m invalid character '$'\n"
         "  $23xyz\n  \e[32m^\e[39m\n"));
    TEST(assertNull(error.cause));
    deleteSource(&src);
    strFree(&msg);
  }

  {
    Source src = sourceFromString("  /* *\n/");
    string msg = generateError(&src, loc(1, 3), loc(1, 3), loc(2, 1),
                               "unclosed multi-line comment");
    Error error = createError(loc(1, 3), msg, NULL);
    TEST(assertEqualInt(error.location.line, 1));
    TEST(assertEqualInt(error.location.pos, 3));
    TEST(assertEqualStr(error.message,
         "<cstring>:1:3: \e[31mError:\e[39m unclosed multi-line comment\n"
         "  /* *\n  \e[32m^~~~~\e[39m\n"));
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
    string msg = generateError(&src, loc(1, 3), loc(1, 6), loc(1, 8),
                               "invalid integer character '%c'", 'x');
    Error cause = createError(loc(1, 6), msg, NULL);
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


static TestResult testMessages() {
  TestResult result = {};

  {
    Source src = sourceFromString("  123xyz  // error");
    string msg = generateError(&src, loc(1, 3), loc(1, 6), loc(1, 8),
                               "invalid integer character '%c'", 'x');
    TEST(assertTrue(msg.owned));
    TEST(assertEqualStr(msg,
         "<cstring>:1:6: \e[31mError:\e[39m invalid integer character 'x'\n"
         "  123xyz  // error\n  \e[32m~~~^~~\e[39m\n"));
    strFree(&msg);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("  $23xyz  // error");
    string msg = generateError(&src, loc(1, 3), loc(1, 3), loc(1, 3),
                               "invalid character '%c'", '$');
    TEST(assertTrue(msg.owned));
    TEST(assertEqualStr(msg,
         "<cstring>:1:3: \e[31mError:\e[39m invalid character '$'\n"
         "  $23xyz  // error\n  \e[32m^\e[39m\n"));
    strFree(&msg);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("  /* *\n/");
    string msg = generateError(&src, loc(1, 3), loc(1, 3), loc(2, 1),
                               "unclosed multi-line comment");
    TEST(assertTrue(msg.owned));
    TEST(assertEqualStr(msg,
         "<cstring>:1:3: \e[31mError:\e[39m unclosed multi-line comment\n"
         "  /* *\n  \e[32m^~~~~\e[39m\n"));
    strFree(&msg);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("f(\n  x, 123");
    string msg = generateError(&src, loc(1, 2), loc(2, 9), loc(2, 9),
                               "missing closing ')'");
    TEST(assertTrue(msg.owned));
    TEST(assertEqualStr(msg,
         "<cstring>:2:9: \e[31mError:\e[39m missing closing ')'\n"
         "  x, 123\n\e[32m~~~~~~~~^\e[39m\n"));
    strFree(&msg);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("()");
    string msg = generateNote(&src, loc(1, 1), loc(1, 1), loc(1, 2),
                              "empty parentheses");
    TEST(assertTrue(msg.owned));
    TEST(assertEqualStr(msg,
         "<cstring>:1:1: \e[90mNote:\e[39m empty parentheses\n"
         "()\n\e[32m^~\e[39m\n"));
    strFree(&msg);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("print()");
    string msg = generateWarning(&src, loc(1, 1), loc(1, 1), loc(1, 5),
                                 "unknown function");
    TEST(assertTrue(msg.owned));
    TEST(assertEqualStr(msg,
         "<cstring>:1:1: \e[35mWarning:\e[39m unknown function\n"
         "print()\n\e[32m^~~~~\e[39m\n"));
    strFree(&msg);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("i += 1");
    string msg = generateHint(&src, loc(1, 1), loc(1, 1), loc(1, 6),
                              "can be replaced with increment '++'");
    TEST(assertTrue(msg.owned));
    TEST(assertEqualStr(msg,
         "<cstring>:1:1: \e[33mHint:\e[39m can be replaced with increment '++'\n"
         "i += 1\n\e[32m^~~~~~\e[39m\n"));
    strFree(&msg);
    deleteSource(&src);
  }

  return result;
}


TestResult error_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<error>", "Test errors.");
  addTest(&suite, &testCreation, "testCreation");
  addTest(&suite, &testDeletion, "testDeletion");
  addTest(&suite, &testMessages, "testMessages");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
