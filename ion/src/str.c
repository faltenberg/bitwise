#include "str.h"

#include <string.h>


string stringFromArray(const char* s) {
  return (string){ .len=strlen(s), .chars=s };
}


string stringFromRange(const char* start, const char* end) {
  return (string){ .len=(end-start < 0 ? 0 : end-start), .chars=start };
}


bool strequal(string a, string b) {
  bool equal = a.len == b.len;
  for (int i = 0; equal && i < a.len; i++) {
    equal = a.chars[i] == b.chars[i];
  }

  return equal;
}


bool cstrequal(string a, const char* b) {
  return strequal(a, stringFromArray(b));
}
