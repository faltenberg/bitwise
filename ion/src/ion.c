#include "sbuffer.h"
#include "ast.h"
#include "astprinter.h"

#include <stdio.h>


int main(int argc, char** argv) {
  ASTNode* node = createExprCall(createExprName("f"), 2, createExprInt(1), createExprInt(2));
  printlnAST(node);
  deleteNode(&node);
  return 0;
}
