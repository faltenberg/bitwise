#include "strintern.h"
#include "sbuffer.h"

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

static SBUF(char*) internedStrings = NULL;


string strintern(const char* string) {
  return strinternRange(string, string + strlen(string));
}


string strinternRange(const char* start, const char* end) {
  size_t length = (end - start < 0) ? 0 : end - start;

  // return already interned string if possible
  for (char** it = internedStrings; it != bufEnd(internedStrings); it++) {
    bool isSubstring = true;
    bool subIdx = 0;
    size_t s = 0;

    // iterate over interned string and check if it contains the new string
    for (size_t i = 0; i < strlen(*it) && s < length; i++) {
      if (start[s] == (*it)[i]) {
        s++;
        if (!isSubstring) {
          isSubstring = true;
          subIdx = i;
        }
      } else {
        isSubstring = false;
        s = 0;
      }
    }

    if (isSubstring && s == length) {
      return (string){ .chars=(*it)+subIdx, .len=length };
    }
  }

  // create copy of new string and intern it
  char* chars = (char*) malloc(length * sizeof(char));
  memcpy(chars, start, length);
  chars[length] = '\0';
  bufPush(internedStrings, chars);
  return (string){ .chars=chars, .len=length };
}


void strinternFree() {
  for (char** it = internedStrings; it != bufEnd(internedStrings); it++) {
    free(*it);
  }
  bufFree(internedStrings);
}
