#ifndef __CUNIT_H__
#define __CUNIT_H__

#include <stdbool.h>
#include <stddef.h>


/******************************************** STRUCTS ********************************************/


typedef struct TestResult {
  int failedTests;
  int totalTests;
} TestResult;

TestResult unite(TestResult a, TestResult b);

void apply(TestResult* result, bool testResult);


typedef TestResult (*TEST_FN)();

typedef struct Test {
  const char* name;
  TEST_FN     fn;
} Test;


typedef enum PrintLevel {
  SILENT, SPARSE, VERBOSE
} PrintLevel;

void printVerbose(const char* format, ...);

void printSparse(const char* format, ...);

void printAlways(const char* format, ...);


typedef struct TestSuite {
  const char* name;
  const char* description;
  int         numTests;
  Test*       tests;
} TestSuite;

TestSuite newSuite(const char* name, const char* description);

void deleteSuite(TestSuite* suite);

void addTest(TestSuite* suite, TEST_FN fn, const char* name);

TestResult run(const TestSuite* suite, PrintLevel verbosity);


/****************************************** ASSERTIONS *******************************************/


#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RST "\x1B[0m"

#define __PROMPT " %s:%-3d ... "

#define __ABORT printVerbose(__PROMPT RED "ABORT\n" RST, __FILE__, __LINE__); return result;
#define ABORT(a) { bool __r = (a); apply(&result, __r); if (!__r) { __ABORT } }

#define TEST(a) apply(&result, (a));

#define SKIP(a) printVerbose(__PROMPT YEL "SKIPPED\n" RST, __FILE__, __LINE__);

#define FAIL(m) apply(&result, 0); printVerbose(__PROMPT RED "FAIL: " RST "%s\n", __FILE__, __LINE__, m);

#define INFO(m) printVerbose(__PROMPT BLU "INFO: " RST "%s\n", __FILE__, __LINE__, m);


#define assertFalse(val) __assertFalse(__FILE__, __LINE__, val)
bool __assertFalse(const char* file, int line, bool cond);


#define assertTrue(val) __assertTrue(__FILE__, __LINE__, val)
bool __assertTrue(const char* file, int line, bool cond);


#define assertEqualBool(val, exp) __assertEqualBool(__FILE__, __LINE__, val, exp)
bool __assertEqualBool(const char* file, int line, bool value, bool expected);


#define assertNotEqualBool(val, exp) __assertNotEqualBool(__FILE__, __LINE__, val, exp)
bool __assertNotEqualBool(const char* file, int line, bool value, bool expected);


#define assertEqualInt(val, exp) __assertEqualInt(__FILE__, __LINE__, val, exp)
bool __assertEqualInt(const char* file, int line, int value, int expected);


#define assertNotEqualInt(val, exp) __assertNotEqualInt(__FILE__, __LINE__, val, exp)
bool __assertNotEqualInt(const char* file, int line, int value, int expected);


#define assertEqualChar(val, exp) __assertEqualChar(__FILE__, __LINE__, val, exp)
bool __assertEqualChar(const char* file, int line, char value, char expected);


#define assertNotEqualChar(val, exp) __assertNotEqualChar(__FILE__, __LINE__, val, exp)
bool __assertNotEqualChar(const char* file, int line, char value, char expected);


#define assertEqualSize(val, exp) __assertEqualSize(__FILE__, __LINE__, val, exp)
bool __assertEqualSize(const char* file, int line, size_t value, size_t expected);


#define assertNotEqualSize(val, exp) __assertNotEqualSize(__FILE__, __LINE__, val, exp)
bool __assertNotEqualSize(const char* file, int line, size_t value, size_t expected);


#define assertEqualFloat(val, exp) __assertEqualFloat(__FILE__, __LINE__, val, exp)
bool __assertEqualFloat(const char* file, int line, float value, float expected);


#define assertNotEqualFloat(val, exp) __assertNotEqualFloat(__FILE__, __LINE__, val, exp)
bool __assertNotEqualFloat(const char* file, int line, float value, float expected);


#define assertEqualDouble(val, exp) __assertEqualDouble(__FILE__, __LINE__, val, exp)
bool __assertEqualDouble(const char* file, int line, double value, double expected);


#define assertNotEqualDouble(val, exp) __assertNotEqualDouble(__FILE__, __LINE__, val, exp)
bool __assertNotEqualDouble(const char* file, int line, double value, double expected);


#define assertEqualString(str, exp) __assertEqualString(__FILE__, __LINE__, str, exp)
bool __assertEqualString(const char* file, int line, const char* string, const char* expected);


#define assertNotEqualString(str, exp) __assertNotEqualString(__FILE__, __LINE__, str, exp)
bool __assertNotEqualString(const char* file, int line, const char* string, const char* expected);


#define assertNull(ptr) __assertNull(__FILE__, __LINE__, ptr)
bool __assertNull(const char* file, int line, const void* pointer);


#define assertNotNull(ptr) __assertNotNull(__FILE__, __LINE__, ptr)
bool __assertNotNull(const char* file, int line, const void* pointer);


#define assertSame(ptr, exp) __assertSame(__FILE__, __LINE__, ptr, exp)
bool __assertSame(const char* file, int line, const void* pointer, const void* expected);


#define assertNotSame(ptr, exp) __assertNotSame(__FILE__, __LINE__, ptr, exp)
bool __assertNotSame(const char* file, int line, const void* pointer, const void* expected);


#define assertEqualMemory(ptr, exp, len) __assertEqualMemory(__FILE__, __LINE__, ptr, exp, len)
bool __assertEqualMemory(const char* file, int line,
                         const void* pointer, const void* expected, size_t length);


#endif // __CUNIT_H__
