#include <stdio.h>


typedef enum ErrorCode {
  SUCCESS,
  ARG_ERROR,
  FILE_ERROR,
} ErrorCode;


int main() {
  const char* src = "x + 10*y";
  printf("parse \"%s\"\n", src);
  return SUCCESS;
}

