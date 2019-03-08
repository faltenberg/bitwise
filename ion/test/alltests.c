#include "cunit.h"

#include <stdio.h>


extern TestResult sbuffer_alltests(PrintLevel);
extern TestResult str_alltests(PrintLevel);
extern TestResult strintern_alltests(PrintLevel);


int main() {
  TestResult result = {};
  result = unite(result, sbuffer_alltests(SPARSE));
  result = unite(result, str_alltests(SPARSE));
  result = unite(result, strintern_alltests(SPARSE));
  printResult(result);
  return 0;
}
