#include "cunit/cunit.h"


extern TestResult sbuffer_alltests();


int main() {
  TestResult result = {};
  result = unite(result, sbuffer_alltests());
  printf("Result: %d of %d tests failed\n", result.failedTests, result.totalTests);
  return 0;
}
