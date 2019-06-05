#include "astprinter.h"

#include <string.h>


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
        default:
          return stringFromArray("todo");
      } break;
    default:
      return stringFromArray("todo");
  }
}
