#include "astprinter.h"

#include <string.h>


string printASTExpr(const ASTNode* node) {
  switch (node->expr.kind) {
    case EXPR_NONE:
      return stringFromArray("(none)");
    case EXPR_INT:
      return toDecString(node->expr.value);
    case EXPR_NAME:
      return stringFromPrint("%.*s", node->expr.name.len, node->expr.name.chars);
    case EXPR_UNOP:
      {
        string op = node->expr.op;
        string expr = printAST(node->expr.rhs);
        string msg = stringFromPrint("(%.*s %.*s)", op.len, op.chars, expr.len, expr.chars);
        strFree(&expr);
        return msg;
      }
    case EXPR_BINOP:
      {
        string lhs = printAST(node->expr.lhs);
        string op = node->expr.op;
        string rhs = printAST(node->expr.rhs);
        string msg = stringFromPrint("(%.*s %.*s %.*s)",
                                     op.len, op.chars, lhs.len, lhs.chars, rhs.len, rhs.chars);
        strFree(&lhs);
        strFree(&rhs);
        return msg;
      }
    default:
      return stringFromArray("todo");
  }
}


string printAST(const ASTNode* node) {
  switch (node->kind) {
    case AST_NONE:
      return stringFromArray("(none)");
    case AST_ERROR:
      {
        // get first line of the error message, therefore simply replace the first '\n' with '\0'
        char* pos = (char*) strchr(node->messages[0].chars, '\n');
        char old = *pos;
        *pos = '\0';
        string msg = stringFromPrint("(error %s)", node->messages[0].chars);
        *pos = old;
        return msg;
      }
    case AST_EXPR:
      return printASTExpr(node);
    default:
      return stringFromArray("todo");
  }
}
