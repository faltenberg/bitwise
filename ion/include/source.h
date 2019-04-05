#ifndef __SOURCE_H__
#define __SOURCE_H__

#include "str.h"
#include "sbuffer.h"


typedef struct Source {
  string           fileName;
  const SBUF(char) content;
} Source;


Source fromString(const char* src);


#endif  // __SOURCE_H__
