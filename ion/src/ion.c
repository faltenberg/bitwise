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

  Source src = sourceFromString("x + y");
  if (src.status != SOURCE_OK) {
    printf("cannot read source\n");
    deleteSource(&src);
    return FILE_ERROR;
  }

  ASTNode* node = parse(&src);

  deleteNode(node);
  deleteSource(&src);
  return SUCCESS;
}

