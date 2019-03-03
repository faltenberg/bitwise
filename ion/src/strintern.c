#include "strintern.h"
#include "sbuffer.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>


typedef struct String {
  size_t      length;
  const char* chars;
} String;


static SBUF(String) _strings = NULL;


const char* strintern(const char* string) {
  return strinternRange(string, string + strlen(string));
}


const char* strinternRange(const char* start, const char* end) {
  size_t length = end - start;

  for (String* s = _strings; s != bufEnd(_strings); s++) {
    if (length == s->length && strncmp(start, s->chars, length) == 0) {
      return s->chars;
    }
  }

  char* chars = (char*) malloc((length + 1) * sizeof(char));
  memcpy(chars, start, length);
  chars[length] = '\0';
  String s = { .length=length, .chars=chars };
  bufPush(_strings, s);
  return chars;
}


void strinternFree() {
  for (String* s = _strings; s != bufEnd(_strings); s++) {
    free((char*) s->chars);
  }
  bufFree(_strings);
}
