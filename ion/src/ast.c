#include "ast.h"


#define CASE(e)  case e: return #e;


const char* strASTKind(ASTKind kind) {
  switch (kind) {
    CASE(AST_NONE);
    CASE(AST_ERROR);
    CASE(AST_EXPR);
  }
}


const char* strExprKind(ExprKind kind) {
  switch (kind) {
    CASE(EXPR_NONE);
    CASE(EXPR_INT);
    CASE(EXPR_NAME);
    CASE(EXPR_PAREN);
    CASE(EXPR_UNOP);
    CASE(EXPR_BINOP);
  }
}


void deleteNode(ASTNode* node) {
  for (int i = 0; i < sbufLength(node->messages); i++) {
    strFree(&node->messages[i]);
  }
  sbufFree(node->messages);

  switch (node->kind) {
    case AST_NONE:
      break;
    case AST_ERROR:
      deleteNode(node->faultyNode);
      break;
    case AST_EXPR:
      switch (node->expr.kind) {
        case EXPR_NONE:
        case EXPR_INT:
        case EXPR_NAME:
          break;
        case EXPR_PAREN:
          deleteNode(node->expr.expr);
          break;
        case EXPR_UNOP:
          deleteNode(node->expr.rhs);
        case EXPR_BINOP:
//TODO:          deleteNode(node->expr.lhs);
//TODO:          deleteNode(node->expr.rhs);
          break;
      } break;
  }

  free(node);
}
