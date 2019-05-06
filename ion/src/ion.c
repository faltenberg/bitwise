#include <stdio.h>

#include "str.h"
#include "source.h"

#include "parser.h"


typedef enum ErrorCode {
  SUCCESS,
  ARG_ERROR,
  FILE_ERROR,
} ErrorCode;


int main() {
  char buffer[5];
  int count = snprintf(buffer, 5, "%d", 12345);
//  buffer[4] = '\0';
  printf("out: %s, count: %d\n", buffer, count);

  Source src = fromString("a * b + c * d");
  ASTNode* ast = parse(src);
  printAST(ast);
  deleteNode(ast);
  return SUCCESS;
}

