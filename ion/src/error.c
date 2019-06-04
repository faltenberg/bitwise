#include "error.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>


#define RED "\e[31m"
#define GRN "\e[32m"
#define YEL "\e[33m"
#define BLU "\e[34m"
#define MGT "\e[35m"
#define CYN "\e[36m"
#define GRY "\e[90m"
#define RST "\e[39m"

#define MIN(a, b) ((a) <= (b) ? (a) : (b))
#define MAX(a, b) ((a) >= (b) ? (a) : (b))


Error createError(Location loc, string message, Error* cause) {
  return (Error){ .location=loc, .message=message, .cause=cause };
}


void deleteError(Error* error) {
  error->location = loc(0, 0);
  strFree(&error->message);
  if (error->cause != NULL) {
    deleteError(error->cause);
    error->cause = NULL;
  }
}


static string generateMessage(const Source* src, Location start, Location caret, Location end,
                              const char* topicColor, const char* topic, const char* description) {
  string message = { .len=0, .owned=true };
  string fileName = src->fileName;
  string line = getLine(src, caret.line);
  line.len = (line.chars[line.len-1] == '\n') ? line.len - 1 : line.len;
  const char* spaces = "                                                                      ";
  const char* tildes = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
  int intend = MAX(0, start.pos - 1);
  intend = (start.line < caret.line) ? 0 : intend;
  int pre = MAX(0, caret.pos - start.pos);
  pre = (start.line < caret.line) ? caret.pos-1 : pre;
  int post = MAX(0, end.pos - caret.pos);
  post = (caret.line < end.line) ? line.len - intend : post;
  {
    again:
    message.chars = (char*) malloc(message.len);
    int count = snprintf((char*) message.chars, message.len,
                         "%.*s:%d:%d: %s%s:"RST" %s\n"
                         "%.*s\n"
                         "%.*s"GRN"%.*s^%.*s"RST"\n",
                         fileName.len, fileName.chars, caret.line, caret.pos,
                         topicColor, topic, description,
                         line.len, line.chars,
                         intend, spaces, pre, tildes, post, tildes);
    if (count >= message.len) {
      free((char*) message.chars);
      message.len = count + 1;
      goto again;
    }
  }
  --message.len;  // it counted bytes for the total message including the '\0'

  return message;
}


#define GENERATE(TOPIC, COLOR)                                                                 \
string generate##TOPIC(const Source* src, Location start, Location caret, Location end,        \
                     const char* format, ...) {                                                \
  string description = { .len=0, .owned=true };                                                \
  {                                                                                            \
    va_list args;                                                                              \
    again:                                                                                     \
    description.chars = (char*) malloc(description.len);                                       \
    va_start(args, format);                                                                    \
    int count = vsnprintf((char*) description.chars, description.len, format, args);           \
    if (count >= description.len) {                                                            \
      free((char*) description.chars);                                                         \
      description.len = count + 1;                                                             \
      goto again;                                                                              \
    }                                                                                          \
    va_end(args);                                                                              \
  }                                                                                            \
  --description.len;                                                                           \
  string message = generateMessage(src, start, caret, end, COLOR, #TOPIC, description.chars);  \
  strFree(&description);                                                                       \
  return message;                                                                              \
}                                                                                              \


GENERATE(Error, RED)
GENERATE(Note, GRY)
GENERATE(Warning, MGT)
GENERATE(Hint, YEL)
