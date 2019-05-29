#include "error.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>


#define RED "\e[31m"
#define GRN "\e[32m"
#define BLU "\e[34m"
#define RST "\e[39m"

#define MIN(a, b) ((a) <= (b) ? (a) : (b))
#define MAX(a, b) ((a) >= (b) ? (a) : (b))

  
Error createError(Location loc, string message, Error* cause) {
  return (Error){ .message=message, .location=loc, .cause=cause };
}


Error generateError(Location loc, const Source* src, Location start, Location end,
                    const char* format, ...) {
  Error error = (Error){ .message=(string){ .len=0, .owned=true, .chars="" },
                         .location=loc, .cause=NULL };

  string message = error.message;
  {
    va_list args;
    again0:
    message.chars = (char*) malloc(message.len);
    va_start(args, format);
    int count = vsnprintf((char*) message.chars, message.len, format, args);
    if (count >= message.len) {
      free((char*) message.chars);
      message.len = count + 1;
      goto again0;
    }
    va_end(args);
  }

  string fileName = src->fileName;
  string line = getLine(src, error.location.line);
  line.len = (line.chars[line.len-1] == '\n') ? line.len - 1 : line.len;
  const char* spaces = "                                                                      ";
  const char* tildes = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
  int intend = MAX(0, start.pos - 1);
  int pre = MAX(0, error.location.pos - start.pos);
  int post = MAX(0, end.pos - error.location.pos);
  {
    again1:
    error.message.chars = (char*) malloc(error.message.len);
    int count = snprintf((char*) error.message.chars, error.message.len,
                        "%.*s:%d:%d: "RED"error:"RST" %.*s\n"
                        "%.*s\n"
                        "%.*s"GRN"%.*s%s%.*s"RST"\n",
                        fileName.len, fileName.chars, error.location.line, error.location.pos,
                        message.len, message.chars,
                        line.len, line.chars,
                        intend, spaces, pre, tildes, "^", post, tildes);
    if (count >= error.message.len) {
      free((char*) error.message.chars);
      error.message.len = count + 1;
      goto again1;
    }
    free((char*) message.chars);
  }
  --error.message.len;  // it counted bytes for the total message including the '\0'

  return error;
}


void deleteError(Error* error) {
  error->location = loc(0, 0);
  strFree(&error->message);
  if (error->cause != NULL) {
    deleteError(error->cause);
    error->cause = NULL;
  }
}
