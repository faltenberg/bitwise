#include "str.h"

#include <string.h>


string fromCString(const char* s) {
  return (string){ .chars=s, .len=strlen(s) };
}


string fromRange(const char* start, const char* end) {
  return (string){ .chars=start, .len=(end-start < 0 ? 0 : end-start) };
}


bool strequal(string a, string b) {
  bool equal = a.len == b.len;
  for (int i = 0; equal && i < a.len; i++) {
    equal = a.chars[i] == b.chars[i];
  }
  return equal;
}


bool cstrequal(string a, const char* b) {
  return strequal(a, fromCString(b));
}
