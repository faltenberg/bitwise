#ifndef __UTIL_H__
#define __UTIL_H__

#include "cunit.h"

#include <stdbool.h>
#include <stdio.h>

#include "str.h"
#include "number.h"


#ifndef FILENAME
#define FILENAME "__test__.tmp"
#endif


#define WRITE_FILE(s)                      \
  do {                                     \
    bool __ok = writeFile(FILENAME, s);    \
    if (!__ok) {                           \
      FAIL("could not write test file");   \
      DELETE_FILE();                       \
      ABORT(0);                            \
    }                                      \
  } while (0);                             \

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


#define assertEqualStr(s, cs)  __assertEqualStr(__FILE__, __LINE__, s, cs)
static bool __assertEqualStr(const char* file, int line, string s, const char* cs) {
  printVerbose(__PROMPT, file, line);
  string exp = stringFromArray(cs);

  int index = 0;
  bool equal = s.len == exp.len;

  if (!equal) {
    printVerbose(RED "ERROR: " RST);
    printVerbose("in \"%.*s\"%s expected length [%d] == [%d]\n",
                 (s.len <= 5 ? s.len : 5), s.chars, (s.len > 5 ? "~" : ""), s.len, exp.len);
    return false;
  }

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

  if (false) {  // TODO
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected Number [%.*s] == [%.*s] (%s)\n",
                 sbufLength(num.value), num.value, sbufLength(exp.value), exp.value,
                 "not implemented");
    return false;
  }
}


#endif  // __UTIL_H__
