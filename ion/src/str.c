#include "str.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>


string stringFromArray(const char* s) {
  return (string){ .len=strlen(s), .owned=false, .chars=s };
}


string stringFromRange(const char* start, const char* end) {
  return (string){ .len=(end-start < 0 ? 0 : end-start), .chars=start };
}


/**
 * `stringFromPrint()` will try to print into a zero-length buffer to determine the necessary size.
 * It will then jump back and allocate enough memory to actually print the string.
 */
string stringFromPrint(const char* format, ...) {
  char* buffer = NULL;
  int size = 0;
  va_list args;
  again:
  buffer = (char*) malloc(size);
  va_start(args, format);
  int count = vsnprintf(buffer, size, format, args);
  if (count >= size) {
    free(buffer);
    size = count + 1;
    goto again;
  }
  va_end(args);
  return (string){ .len=count, .owned=true, .chars=buffer };
}


void strFree(string* s) {
  if (s->owned) {
    free((char*) s->chars);
  }
  s->len = 0;
  s->chars = "";
  s->owned = false;
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
