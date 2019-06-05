#ifndef __UTIL_H__
#define __UTIL_H__


/**
 * Test Util
 * =========
 *
 * This header contains some useful tools to simplify some common tasks during automatic testing as
 * well as additional assert functions that are not provided by CUnit.
 *
 *
 * File Support
 * ------------
 *
 * Sometimes it is necessary to test file import or export. The `WRITE_FILE()` writes some text to
 * a file, that can be read by a unit test. The file name is set by defining `FILENAME`. If the
 * constant is not defined, a default name is used. At the end the file should be deleted with
 * `DELETE_FILE()`.
 *
 * ```c {.line-numbers}
 * #define FILENAME "test_input.txt"  // optional, will default
 * #include "util.h"
 *
 * int main() {
 *  WRITE_FILE("lorem ipsum dolor");
 *  // TODO: read file given by FILENAME
 *  // TODO: test file content
 *  DELETE_FILE();
 * }
 * ```
 *
 *
 * Assert Enums
 * ------------
 *
 * For testing the equality of enums, there exit a generator to create an assert function for a
 * some enum. There must exit a `strMyEnum()` function that returns a string for a each enum value.
 *
 * ```c {.line-numbers}
 * #include "util.h"
 *
 * typedef enum MyEnum {
 *   OPTION0, OPTION1
 * } MyEnum;
 *
 * const char* strMyEnum(MyEnum e) {
 *   return (e == OPTION0) ? "OPTION0" : "OPTION1";
 * }
 *
 * GENERATE_ASSERT_EQUAL_ENUM(MyEnum)
 *
 * int main() {
 *   MyEnum e = OPTION0;
 *   assertEqualEnum(MyEnum, e, OPTION0);
 *   assertNotEqualEnum(MyEnum, e, OPTION1);
 * }
 * ```
 */


#include "cunit.h"

#include <stdbool.h>
#include <stdio.h>

#include "str.h"
#include "number.h"


#ifndef FILENAME
#define FILENAME "__test__.tmp"
#endif


#define WRITE_FILE(s)                     \
  do {                                    \
    bool __ok = writeFile(FILENAME, s);   \
    if (!__ok) {                          \
      FAIL("could not write test file");  \
      DELETE_FILE();                      \
      ABORT(0);                           \
    }                                     \
  } while (0);                            \

static bool writeFile(const char* fileName, const char* content) {
  FILE* file = fopen(fileName, "w");
  if (file == NULL) {
    fclose(file);
    return false;
  }

  if (fputs(content, file) == EOF) {
    fclose(file);
    return false;
  }

  fclose(file);
  return true;
}


#define DELETE_FILE()                             \
  do {                                            \
    bool __ok = deleteFile(FILENAME);             \
    if (!__ok) {                                  \
      INFO("check if " FILENAME " was deleted");  \
    }                                             \
  } while (0);                                    \

static bool deleteFile(const char* fileName) {
  return remove(fileName) == 0;
}


#define assertEqualEnum(KIND, k, e)  __assertEqual##KIND(__FILE__, __LINE__, k, e)
#define __assertEqualEnum(KIND, f, l, k, e)  __assertEqual##KIND(f, l, k, e)
#define assertNotEqualEnum(KIND, k, e)  __assertNotEqual##KIND(__FILE__, __LINE__, k, e)
#define __assertNotEqualEnum(KIND, f, l, k, e)  __assertNotEqual##KIND(f, l, k, e)

#define GENERATE_ASSERT_EQUAL_ENUM(KIND)                                               \
static bool __assertEqual##KIND(const char* file, int line, KIND kind, KIND exp) {     \
  printVerbose(__PROMPT, file, line);                                                  \
  if (kind == exp) {                                                                   \
    printVerbose(GRN "OK\n" RST);                                                      \
    return true;                                                                       \
  } else {                                                                             \
    printVerbose(RED "ERROR: " RST);                                                   \
    printVerbose("expected enum [%s] == [%s]\n", str##KIND(kind), str##KIND(exp));     \
    return false;                                                                      \
  }                                                                                    \
}                                                                                      \
static bool __assertNotEqual##KIND(const char* file, int line, KIND kind, KIND exp) {  \
  printVerbose(__PROMPT, file, line);                                                  \
  if (kind != exp) {                                                                   \
    printVerbose(GRN "OK\n" RST);                                                      \
    return true;                                                                       \
  } else {                                                                             \
    printVerbose(RED "ERROR: " RST);                                                   \
    printVerbose("expected enum [%s] != [%s]\n", str##KIND(kind), str##KIND(exp));     \
    return false;                                                                      \
  }                                                                                    \
}                                                                                      \



#define assertEqualStr(s, cs)  __assertEqualStr(__FILE__, __LINE__, s, cs)
static bool __assertEqualStr(const char* file, int line, string s, const char* cs) {
  printVerbose(__PROMPT, file, line);
  string exp = stringFromArray(cs);

  if (s.len != exp.len) {
    printVerbose(RED "ERROR: " RST);
    printVerbose("in \"%.*s\"%s expected length [%d] == [%d]\n",
                 (s.len <= 5 ? s.len : 5), s.chars, (s.len > 5 ? "~" : ""), s.len, exp.len);
    return false;
  }

  int index = 0;
  bool equal = true;
  while (index < s.len && equal) {
    equal = s.chars[index] == exp.chars[index];
    if (equal) {
      ++index;
    }
  }

  if (equal) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("in \"%.*s\"%s[%d] expected [%c] == [%c]\n",
                 (s.len <= 5 ? s.len : 5), s.chars, (s.len > 5 ? "~" : ""), index,
                 s.chars[index], exp.chars[index]);
    return false;
  }
}


#define assertEqualNumber(n, exp)  __assertEqualNumber(__FILE__, __LINE__, n, exp)
static bool __assertEqualNumber(const char* file, int line, Number num, Number exp) {
  printVerbose(__PROMPT, file, line);

  if (num.value == exp.value) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected Number [%d] == [%d] (%s)\n",
                 num.value, exp.value, "not implemented");
    return false;
  }
}


#endif  // __UTIL_H__
