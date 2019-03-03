#include "cunit.h"

#include <stdio.h>


extern TestResult selftest_alltests(PrintLevel);
extern TestResult template_alltests(PrintLevel);


int main() {
  TestResult result = {};
  result = unite(result, selftest_alltests(VERBOSE));
  result = unite(result, template_alltests(SPARSE));
  printf("Result: %d of %d tests failed\n", result.failedTests, result.totalTests);
  return 0;
}
