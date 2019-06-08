#include "strintern.h"
#include "sbuffer.h"
#include "arena.h"

#include <string.h>


static SBUF(char*) internedStrings = NULL;
static Arena       allocator       = {};


string strintern(const char* string) {
  return strinternRange(string, string + strlen(string));
}


string strinternRange(const char* start, const char* end) {
  size_t length = (end - start < 0) ? 0 : end - start;

  // return already interned string if possible
  for (char** it = internedStrings; it != sbufEnd(internedStrings); it++) {
    bool isSubstring = true;
    size_t subIdx = 0;
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
  char* chars = (char*) arenaAlloc(&allocator, length + 1);
  memcpy(chars, start, length);
  chars[length] = '\0';
  sbufPush(internedStrings, chars);
  return (string){ .chars=chars, .len=length, .owned=false };
}


void strinternFree() {
  sbufFree(internedStrings);
  arenaFree(&allocator);
}
