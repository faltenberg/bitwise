#include "cunit.h"

#include <stdio.h>


extern TestResult sbuffer_alltests(PrintLevel);
extern TestResult strintern_alltests(PrintLevel);
extern TestResult lexer_alltests(PrintLevel);


int main() {
  TestResult result = {};
  result = unite(result, sbuffer_alltests(SPARSE));
  result = unite(result, strintern_alltests(SPARSE));
  result = unite(result, lexer_alltests(SUMMARY));
  printf("Result: %d of %d tests failed\n", result.failedTests, result.totalTests);
  return 0;
}
