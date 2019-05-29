#include "cunit.h"


extern TestResult sbuffer_alltests(PrintLevel);
extern TestResult arena_alltests(PrintLevel);
extern TestResult str_alltests(PrintLevel);
extern TestResult strintern_alltests(PrintLevel);
extern TestResult source_alltests(PrintLevel);
extern TestResult error_alltests(PrintLevel);
extern TestResult lexer_alltests(PrintLevel);
extern TestResult number_alltests(PrintLevel);
extern TestResult parser_alltests(PrintLevel);


int main() {
  TestResult result = {};
  result = unite(result, sbuffer_alltests(SPARSE));
  result = unite(result, arena_alltests(VERBOSE));
  result = unite(result, str_alltests(SPARSE));
  result = unite(result, strintern_alltests(SPARSE));
  result = unite(result, error_alltests(SPARSE));
  result = unite(result, source_alltests(SPARSE));
  result = unite(result, lexer_alltests(SUMMARY));
  result = unite(result, number_alltests(VERBOSE));
  result = unite(result, parser_alltests(VERBOSE));
  printResult(result);
}
