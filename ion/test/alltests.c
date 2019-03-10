#include "cunit.h"

#include <stdio.h>


extern TestResult sbuffer_alltests(PrintLevel);
extern TestResult str_alltests(PrintLevel);
extern TestResult strintern_alltests(PrintLevel);
extern TestResult arena_alltests(PrintLevel);


int main() {
  TestResult result = {};
  result = unite(result, sbuffer_alltests(SPARSE));
  result = unite(result, str_alltests(SPARSE));
  result = unite(result, strintern_alltests(SPARSE));
//  result = unite(result, arena_alltests(VERBOSE));
  printResult(result);
  return 0;
}
