#include <stdio.h>

#include "str.h"


typedef enum ErrorCode {
  SUCCESS,
  ARG_ERROR,
  FILE_ERROR,
} ErrorCode;


int main() {
  string src = fromCString("x + 10*y");
  printf("parse \"%.*s\"\n", src.len, src.chars);
  return SUCCESS;
}

