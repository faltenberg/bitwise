#include "cunit.h"

#include "source.h"

#define FILENAME "__source__.tmp"
#include "util.h"


static TestResult testCreationFromString() {
  TestResult result = {};

  {
    Source src = sourceFromString("foo bar");
    TEST(assertEqualInt(src.status, SOURCE_OK));
    TEST(assertEqualStr(src.fileName, "<cstring>"));
    TEST(assertNotSame(src.fileName.chars, "<cstring>"));
    TEST(assertEqualStr(src.content, "foo bar"));
    TEST(assertNotSame(src.content.chars, "foo bar"));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("");
    TEST(assertEqualInt(src.status, SOURCE_OK));
    TEST(assertEqualStr(src.fileName, "<cstring>"));
    TEST(assertNotSame(src.fileName.chars, "<cstring>"));
    TEST(assertEqualStr(src.content, ""));
    TEST(assertNotSame(src.content.chars, ""));
    deleteSource(&src);
  }

  return result;
}


static TestResult testCreationFromFile() {
  TestResult result = {};
  WRITE_FILE("lorem ipsum dolor");

  {
    Source src = sourceFromFile(FILENAME);
    TEST(assertEqualInt(src.status, SOURCE_OK));
    TEST(assertEqualStr(src.fileName, FILENAME));
    TEST(assertNotSame(src.fileName.chars, FILENAME));
    TEST(assertEqualStr(src.content, "lorem ipsum dolor"));
    TEST(assertNotSame(src.content.chars, "lorem ipsum dolor"));
    deleteSource(&src);
  }

  {
    const char* name = "." FILENAME;
    Source src = sourceFromFile(name);
    TEST(assertEqualInt(src.status, SOURCE_ERROR));
    TEST(assertEqualStr(src.fileName, name));
    TEST(assertNotSame(src.fileName.chars, name));
    TEST(assertEqualStr(src.content, "ERROR: could not open the file"));
    TEST(assertNotSame(src.content.chars, "ERROR: could not open the file"));
    deleteSource(&src);
  }

  DELETE_FILE();
  return result;
}


static TestResult testDeletion() {
  TestResult result = {};

  {
    Source src = sourceFromString("foo bar");
    deleteSource(&src);
    TEST(assertEqualInt(src.status, SOURCE_NONE));
    TEST(assertEqualStr(src.fileName, ""));
    TEST(assertSame(src.fileName.chars, ""));
    TEST(assertEqualStr(src.content, ""));
    TEST(assertSame(src.content.chars, ""));
  }

  return result;
}


static TestResult testGetLine() {
  TestResult result = {};

  {
    Source src = sourceFromString("foo bar");
    TEST(assertEqualStr(getLine(&src, 0), ""));
    TEST(assertEqualStr(getLine(&src, 1), "foo bar"));
    TEST(assertEqualStr(getLine(&src, 2), ""));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("foo bar\n");
    TEST(assertEqualStr(getLine(&src, 0), ""));
    TEST(assertEqualStr(getLine(&src, 1), "foo bar\n"));
    TEST(assertEqualStr(getLine(&src, 2), ""));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("foo\nbar");
    TEST(assertEqualStr(getLine(&src, 0), ""));
    TEST(assertEqualStr(getLine(&src, 1), "foo\n"));
    TEST(assertEqualStr(getLine(&src, 2), "bar"));
    TEST(assertEqualStr(getLine(&src, 3), ""));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("foo\nbar\nbaz");
    TEST(assertEqualStr(getLine(&src, 0), ""));
    TEST(assertEqualStr(getLine(&src, 1), "foo\n"));
    TEST(assertEqualStr(getLine(&src, 2), "bar\n"));
    TEST(assertEqualStr(getLine(&src, 3), "baz"));
    TEST(assertEqualStr(getLine(&src, 4), ""));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("\n");
    TEST(assertEqualStr(getLine(&src, 0), ""));
    TEST(assertEqualStr(getLine(&src, 1), "\n"));
    TEST(assertEqualStr(getLine(&src, 2), ""));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("\n\n");
    TEST(assertEqualStr(getLine(&src, 0), ""));
    TEST(assertEqualStr(getLine(&src, 1), "\n"));
    TEST(assertEqualStr(getLine(&src, 2), "\n"));
    TEST(assertEqualStr(getLine(&src, 3), ""));
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("");
    TEST(assertEqualStr(getLine(&src, 0), ""));
    TEST(assertEqualStr(getLine(&src, 1), ""));
    TEST(assertEqualStr(getLine(&src, 2), ""));
    deleteSource(&src);
  }

  return result;
}


TestResult source_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<source>", "Test sources.");
  addTest(&suite, &testCreationFromString, "testCreationFromString");
  addTest(&suite, &testCreationFromFile,   "testCreationFromFile");
  addTest(&suite, &testDeletion,           "testDeletion");
  addTest(&suite, &testGetLine,            "testGetLine");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
