#include "cunit.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <stdarg.h>


/******************************************* INTERNAL ********************************************/


#define FPRECISION  0.00001f
#define DPRECISION  0.00000001


/**
 * Compares floats and returns true if their relative distance is within some limit.
 * Special cases: -0.0f==0.0f, nan!=nan.
 * Source: https://www.floating-point-gui.de/errors/comparison/
 */
static bool cmpFloat(float a, float b) {
  float absA = a < 0.0f ? -a : a;
  float absB = b < 0.0f ? -b : b;
  float diff = fabs(a - b);

  if (a == b) {
    return true;  // shortcut, covers infinities and -0.0f
  } else if (a == 0.0f || b == 0.0f || diff < FLT_MIN) {
    return diff < (FPRECISION * FLT_MIN);  // a or b close to zero, relative error thus meaningless
  } else {
    return (diff / fmin(absA+absB, FLT_MAX)) < FPRECISION;  // return relative error
  }
}

static bool cmpDouble(double a, double b) {
  double absA = a < 0.0 ? -a : a;
  double absB = b < 0.0 ? -b : b;
  double diff = fabs(a - b);

  if (a == b) {
    return true;  // shortcut, covers infinities and -0.0
  } else if (a == 0.0 || b == 0.0 || diff < DBL_MIN) {
    return diff < (DPRECISION * DBL_MIN);  // a or b close to zero, relative error thus meaningless
  } else {
    return (diff / fmin(absA+absB, DBL_MAX)) < DPRECISION;  // return relative error
  }
}


/**
 * Returns -1 if strings are equal otherwise the index at which they differ.
 */
static int cmpString(const char* a, const char* b) {
  int index = -1;
  bool equal = true;

  while (equal) {
    equal = *a == *b;
    index++;
    if (*a == '\0' || *b == '\0') break;
    a++; b++;
  }

  return equal ? -1 : index;
}


/**
  * Returns -1 if the memory blocks are equal otherwise the index at which they differ.
  */
static int cmpMemory(const void* a, const void* b, size_t length) {
  int index;
  bool equal = true;

  for (index = 0; equal && index < length; index++) {
    equal = ((unsigned char*) a)[index] == ((unsigned char*) b)[index];
  }
  index--;  // compensate the increment on loop exit

  return equal ? -1 : index;
}


static const char* tostringBool(bool b) {
  return b ? "true" : "false";
}


static const char* tostringChar(char c) {
  static const char* table[] = {
      "'\\0'", "'SOH'", "'STX'", "'ETX'", "'EOT'", "'ENQ'", "'ACK'", "'BEL'",
      "'\\b'", "'\\t'", "'\\n'", "'\\v'", "'\\f'", "'\\r'", "'SO'",  "'SI'",
      "'DLE'", "'DC1'", "'DC2'", "'DC3'", "'DC4'", "'NAK'", "'SYN'", "'ETB'",
      "'CAN'", "'EM'",  "'SUB'", "'ESC'", "'FS'",  "'GS'",  "'RS'",  "'US'",
      "' '",   "'!'",   "'\"'",  "'#'",   "'$'",   "'%'",   "'&'",   "'''",
      "'('",   "')'",   "'*'",   "'+'",   "'´'",   "'-'",   "'.'",   "'/'",
      "'0'",   "'1'",   "'2'",   "'3'",   "'4'",   "'5'",   "'6'",   "'7'",
      "'8'",   "'9'",   "':'",   "';'",   "'<'",   "'='",   "'>'",   "'?'",
      "'@'",   "'A'",   "'B'",   "'C'",   "'D'",   "'E'",   "'F'",   "'G'",
      "'H'",   "'I'",   "'J'",   "'K'",   "'L'",   "'M'",   "'N'",   "'O'",
      "'P'",   "'Q'",   "'R'",   "'S'",   "'T'",   "'U'",   "'V'",   "'W'",
      "'X'",   "'Y'",   "'Z'",   "'['",   "'\\'",  "']'",   "'^'",   "'_'",
      "'`'",   "'a'",   "'b'",   "'c'",   "'d'",   "'e'",   "'f'",   "'g'",
      "'h'",   "'i'",   "'j'",   "'k'",   "'l'",   "'m'",   "'n'",   "'o'",
      "'p'",   "'q'",   "'r'",   "'s'",   "'t'",   "'u'",   "'v'",   "'w'",
      "'x'",   "'y'",   "'z'",   "'{'",   "'|'",   "'}'",   "'~'",   "'DEL'",
  };
  return table[(unsigned char) c];
}


static PrintLevel printLevel;


void printVerbose(const char* format, ...) {
  if (printLevel >= VERBOSE) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
  }
}


void printSparse(const char* format, ...) {
  if (printLevel >= SPARSE) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
  }
}


void printSummary(const char* format, ...) {
  if (printLevel >= SUMMARY) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
  }
}


void printAlways(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
}


/******************************************** STRUCTS ********************************************/


TestResult unite(TestResult a, TestResult b) {
  TestResult result = {};
  result.failedTests = a.failedTests + b.failedTests;
  result.totalTests = a.totalTests + b.totalTests;
  return result;
}


void apply(TestResult* result, bool testResult) {
  result->failedTests += testResult ? 0 : 1;
  result->totalTests += 1;
}


TestSuite newSuite(const char* name, const char* description) {
  TestSuite suite = {};
  suite.name = name;
  suite.description = description;
  suite.numTests = 0;
  suite.tests = NULL;
  return suite;
}


void deleteSuite(TestSuite* suite) {
  free(suite->tests);
  suite->numTests = 0;
  suite->tests = NULL;
}


void addTest(TestSuite* suite, TEST_FN fn, const char* name) {
  Test* temp = (Test*) malloc((suite->numTests + 1) * sizeof(Test));
  if (temp == NULL) {
    printAlways(__PROMPT RED "FATAL: " RST "couldn't allocate memory for test [%s]\n",
                __FILE__, __LINE__, name);
  } else {
    memcpy(temp, suite->tests, suite->numTests * sizeof(Test));
    free(suite->tests);
    suite->tests = temp;
    suite->tests[suite->numTests++] = (Test) { name, fn };
  }
}


TestResult run(const TestSuite* suite, PrintLevel verbosity) {
  PrintLevel temp = printLevel;
  printLevel = verbosity;

  TestResult result = {};
  printSummary(BLU "[%s]" RST " %s\n", suite->name, suite->description);

  for (int i = 0; i < suite->numTests; i++) {
    Test test = suite->tests[i];

    if (printLevel < VERBOSE) {
      printSparse("%s ", test.name);
      for(int i = strlen(test.name); i < 45; i++) printSparse(".");
      printSparse("... ");
    } else {
      printSparse("%s ... \n", test.name);
    }

    TestResult r = test.fn();

    if (printLevel < VERBOSE) {
      if (r.failedTests == 0) {
        printSparse(GRN "OK\n" RST);
      } else {
        printSparse(RED "ERROR:" RST " %d of %d tests failed\n", r.failedTests, r.totalTests);
      }
    }

    result.failedTests += r.failedTests;
    result.totalTests += r.totalTests;
  }

  float passedRatio = result.failedTests / (result.totalTests + FPRECISION);
  passedRatio = 100.0f * (1.0f - passedRatio);
  printSummary(BLU "[%s]" RST " %d of %d tests failed (%.2f%% passed)\n",
              suite->name, result.failedTests, result.totalTests, passedRatio);
  printSummary("\n");

  printLevel = temp;
  return result;
}


/****************************************** ASSERTIONS *******************************************/


bool __assertFalse(const char* file, int line, bool cond) {
  printVerbose(__PROMPT, file, line);
  if (cond == false) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%s]\n", tostringBool(false));
    return false;
  }
}


bool __assertTrue(const char* file, int line, bool cond) {
  printVerbose(__PROMPT, file, line);
  if (cond == true) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%s]\n", tostringBool(true));
    return false;
  }
}


bool __assertEqualBool(const char* file, int line, bool value, bool expected) {
  printVerbose(__PROMPT, file, line);
  if (value == expected) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%s] == [%s]\n", tostringBool(value), tostringBool(expected));
    return false;
  }
}


bool __assertNotEqualBool(const char* file, int line, bool value, bool expected) {
  printVerbose(__PROMPT, file, line);
  if (value != expected) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%s] != [%s]\n", tostringBool(value), tostringBool(expected));
    return false;
  }
}


bool __assertEqualInt(const char* file, int line, int value, int expected) {
  printVerbose(__PROMPT, file, line);
  if (value == expected) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%d] == [%d]\n", value, expected);
    return false;
  }
}


bool __assertNotEqualInt(const char* file, int line, int value, int expected) {
  printVerbose(__PROMPT, file, line);
  if (value != expected) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%d] != [%d]\n", value, expected);
    return false;
  }
}


bool __assertEqualChar(const char* file, int line, char value, char expected) {
  printVerbose(__PROMPT, file, line);
  if (value == expected) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%s] == [%s]\n", tostringChar(value), tostringChar(expected));
    return false;
  }
}


bool __assertNotEqualChar(const char* file, int line, char value, char expected) {
  printVerbose(__PROMPT, file, line);
  if (value != expected) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%s] != [%s]\n", tostringChar(value), tostringChar(expected));
    return false;
  }
}


bool __assertEqualSize(const char* file, int line, size_t value, size_t expected) {
  printVerbose(__PROMPT, file, line);
  if (value == expected) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%lu] == [%lu]\n", value, expected);
    return false;
  }
}


bool __assertNotEqualSize(const char* file, int line, size_t value, size_t expected) {
  printVerbose(__PROMPT, file, line);
  if (value != expected) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%lu] != [%lu]\n", value, expected);
    return false;
  }
}


bool __assertEqualFloat(const char* file, int line, float value, float expected) {
  printVerbose(__PROMPT, file, line);
  if (cmpFloat(value, expected)) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%e] == [%e]\n", value, expected);
    return false;
  }
}


bool __assertNotEqualFloat(const char* file, int line, float value, float expected) {
  printVerbose(__PROMPT, file, line);
  if (!cmpFloat(value, expected)) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%e] != [%e]\n", value, expected);
    return false;
  }
}


bool __assertEqualDouble(const char* file, int line, double value, double expected) {
  printVerbose(__PROMPT, file, line);
  if (cmpDouble(value, expected)) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%e] == [%e]\n", value, expected);
    return false;
  }
}


bool __assertNotEqualDouble(const char* file, int line, double value, double expected) {
  printVerbose(__PROMPT, file, line);
  if (!cmpDouble(value, expected)) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%e] != [%e]\n", value, expected);
    return false;
  }
}


bool __assertEqualString(const char* file, int line, const char* string, const char* expected) {
  printVerbose(__PROMPT, file, line);
  int index = cmpString(string, expected);
  if (index == -1) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("in \"%.5s\"%s[%d] expected [%s] == [%s]\n",
                 string, (strlen(string) > 5 ? "~" : ""), index,
                 tostringChar(string[index]), tostringChar(expected[index]));
    return false;
  }
}


bool __assertNotEqualString(const char* file, int line, const char* string, const char* expected) {
  printVerbose(__PROMPT, file, line);
  int index = cmpString(string, expected);
  if (index != -1) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [\"%.5s\"%s] != [\"%.5s\"%s]\n",
                 string, (strlen(string) > 5 ? "~" : ""), expected,
                 (strlen(expected) > 5 ? "~" : ""));
    return false;
  }
}


bool __assertNull(const char* file, int line, const void* pointer) {
  printVerbose(__PROMPT, file, line);
  if (pointer == NULL) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%p] == [%p]\n", pointer, NULL);
    return false;
  }
}


bool __assertNotNull(const char* file, int line, const void* pointer) {
  printVerbose(__PROMPT, file, line);
  if (pointer != NULL) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%p] != [%p]\n", pointer, NULL);
    return false;
  }
}


bool __assertSame(const char* file, int line, const void* pointer, const void* expected) {
  printVerbose(__PROMPT, file, line);
  if (pointer == expected) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%p] == [%p]\n", pointer, expected);
    return false;
  }
}


bool __assertNotSame(const char* file, int line, const void* pointer, const void* expected) {
  printVerbose(__PROMPT, file, line);
  if (pointer != expected) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    printVerbose(RED "ERROR: " RST);
    printVerbose("expected [%p] != [%p]\n", pointer, expected);
    return false;
  }
}


bool __assertEqualMemory(const char* file, int line,
                         const void* pointer, const void* expected, size_t length) {
  printVerbose(__PROMPT, file, line);
  int index = cmpMemory(pointer, expected, length);
  if (index == -1) {
    printVerbose(GRN "OK\n" RST);
    return true;
  } else {
    const unsigned char* p = (const unsigned char*) pointer;
    const unsigned char* e = (const unsigned char*) expected;
    printVerbose(RED "ERROR: " RST);
    printVerbose("at (%p)[%d] expected [0x%02x] == [0x%02x]\n",
                 pointer, index, p[index], e[index]);
    return false;
  }
}
