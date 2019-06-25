#include <stdio.h>

#include "str.h"
#include "strintern.h"
#include "source.h"
#include "token.h"
#include "parser.h"
#include "astprinter.h"


typedef enum ErrorCode {
  SUCCESS,
  ARG_ERROR,
  FILE_ERROR,
} ErrorCode;


static void printErrors(const ASTNode* node) {
  switch (node->kind) {
    case AST_NONE:
      break;
    case AST_ERROR:
      printErrors(node->faultyNode);
      break;
    case AST_EXPR:
      switch (node->expr.kind) {
        case EXPR_NONE:
          break;
        case EXPR_NAME:
          break;
        case EXPR_INT:
          break;
        case EXPR_UNOP:
          printErrors(node->expr.rhs);
          break;
        case EXPR_BINOP:
          printErrors(node->expr.lhs);
          printErrors(node->expr.rhs);
          break;
        case EXPR_PAREN:
          printErrors(node->expr.expr);
          break;
      }
      break;
  }

  for (int i = 0; i < sbufLength(node->messages); i++) {
    printf("%.*s", node->messages[i].len, node->messages[i].chars);
  }
}


static void compile(const char* input) {
  printf("--------------------\n");
  printf("Compile \"%s\" ...\n", input);
  Source src = sourceFromString(input);
  ASTNode* node = parse(&src);
  printErrors(node);
  string s = printAST(node);
  printf("Result: %.*s\n", s.len, s.chars);
  strFree(&s);
  deleteNode(node);
  deleteSource(&src);
  printf("\n");
}


int main() {
  printf("ION COMPILER\n");

  Source src = sourceFromString("x + y");
  if (src.status != SOURCE_OK) {
    printf("cannot read source\n");
    deleteSource(&src);
    return FILE_ERROR;
  }

  compile("");
  compile(" ");
  compile("123");
  compile("xyz");
  compile("-x");
  compile("+ x");
  compile("+-x");
  compile("$");
  compile("if");
  compile("!if");
  compile("!$");
  compile("/x");
  compile("/if");
  compile("+/x");
  compile("-x;");
  compile("x + y");
  compile("x + y + z");
  compile("x + _ + z");
  compile("x + y $ z");
  compile("x $ y $ z");
  compile("x + -y");
  compile("-x + y");
  compile("x + -y + z");
  compile("x + y + -z");
  compile("!x + ~y + -z");
  compile("x + if");
  compile("x + if () z");
  compile("x + y z");
  compile("x + y // comment");
  compile("x +/*comment*/ y");

  deleteSource(&src);
  strinternFree();
  return SUCCESS;
}
