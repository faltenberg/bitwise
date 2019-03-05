#include "sbuffer.h"
#include "ast.h"
#include "astprinter.h"

#include "strintern.h"
#include <stdio.h>
#include <assert.h>


int main() {
  ASTNode* node = createStmtBlock(1, createStmtContinue());
  printlnAST(node);
  return 0;
}
