#include <stdio.h>

#include "str.h"
#include "source.h"

#include "token.h"
#include "parser.h"


typedef enum ErrorCode {
  SUCCESS,
  ARG_ERROR,
  FILE_ERROR,
} ErrorCode;


int main() {
  printf("ION COMPILER\n");
  return SUCCESS;
}

