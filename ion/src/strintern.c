#include "strintern.h"
#include "sbuffer.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>


typedef struct String {
  size_t      length;
  const char* chars;
} String;


static String* _strings = NULL;


const char* strintern(const char* string) {
  return strinternRange(string, string + strlen(string));
}


const char* strinternRange(const char* start, const char* end) {
  size_t length = end - start;

  for (size_t i = 0; i < bufLength(_strings); i++) {
    if (length == _strings[i].length && strncmp(start, _strings[i].chars, length) == 0) {
      return _strings[i].chars;
    }
  }

  char* chars = (char*) malloc((length + 1) * sizeof(char));
  memcpy(chars, start, length);
  chars[length] = '\0';
  String s = { .length=length, .chars=chars};
  bufPush(_strings, s);
  return chars;
}


void strinternFree() {
  for (size_t i = 0; i < bufLength(_strings); i++) {
    free((char*) _strings[i].chars);
  }
  bufFree(_strings);
}
