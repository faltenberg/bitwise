#include "astprinter.h"

#include <stdio.h>


void printlnType(const Type* type) {
  printType(type);
  printf("\n");
}


void printType(const Type* type) {
  if (type == NULL) {
    printf("null");
    return;
  }

  switch (type->kind) {
    case TYPE_NONE:
      printf("(none)");
      break;
    case TYPE_PRIMITIVE:
      printf("%s", type->name);
      break;
    case TYPE_FUNC:
      printf("(func (");
      for (int i = 0; i < bufLength(type->paramTypes); i++) {
        printType(type->paramTypes[i]);
        if (i < bufLength(type->paramTypes) - 1) {
          printf(", ");
        }
      }
      printf(") -> ");
      printType(type->returnType);
      printf(")");
      break;
    case TYPE_ARRAY:
      printf("(array ");
      printType(type->baseType);
      printf(" %lu)", type->arraySize);
      break;
    case TYPE_POINTER:
      printf("(ptr ");
      printType(type->baseType);
      printf(")");
      break;
    case TYPE_STRUCT:
      printf("(struct %s { ", type->name);
      for (int i = 0; i < bufLength(type->fieldTypes); i++) {
        printType(type->fieldTypes[i]);
        printf(" ");
      }
      printf("})");
      break;
    case TYPE_FIELD:
      printf("%s: ", type->name);
      printType(type->baseType);
      printf(";");
      break;
  }
}


static void printDecl(const ASTNode* decl) {

}


static void printStmt(const ASTNode* stmt) {

}


static const char* optoa(TokenKind op) {
  switch (op) {
    case TOKEN_OP_PLUS:
    case TOKEN_OP_ADD:
      return "+";
    case TOKEN_OP_INC:
      return "++";
    case TOKEN_OP_MINUS:
    case TOKEN_OP_SUB:
      return "-";
    case TOKEN_OP_DEC:
      return "--";
    case TOKEN_OP_MUL:
      return "*";
    case TOKEN_OP_DIV:
      return "/";
    case TOKEN_OP_MOD:
      return "%";
    default:
      return "";
  }
};


static void printExpr(const ASTNode* expr) {
  switch (expr->exprKind) {
    case EXPR_NONE:
      break;
    case EXPR_INT:
      printf("%lu", expr->intValue);
      break;
    case EXPR_BOOL:
      printf("%s", expr->boolValue ? "true" : "false");
      break;
    case EXPR_NAME:
      printf("%s", expr->nameLiteral);
      break;
    case EXPR_UNARY:
      printf("(%s ", optoa(expr->unop));
      printAST(expr->operand);
      printf(")");
      break;
    case EXPR_BINARY:
      printf("(%s ", optoa(expr->binop));
      printAST(expr->left);
      printf(" ");
      printAST(expr->right);
      printf(")");
      break;
    case EXPR_CALL:
      printf("(call ");
      printAST(expr->operand);
      for (int i = 0; i < bufLength(expr->arguments); i++) {
        printf(" ");
        printAST(expr->arguments[i]);
      }
      printf(")");
      break;
    case EXPR_INDEX:
      printf("(index ");
      printAST(expr->operand);
      printf(" ");
      printAST(expr->index);
      printf(")");
      break;
    case EXPR_FIELD:
      printf("(field ");
      printAST(expr->operand);
      printf(" %s)", expr->fieldName);
      break;
  }
}


void printlnAST(const ASTNode* node) {
  printAST(node);
  printf("\n");
}


void printAST(const ASTNode* node) {
  if (node == NULL) {
    printf("(null)");
    return;
  }

  switch (node->kind) {
    case NODE_DECL:
      printDecl(node);
      break;
    case NODE_STMT:
      printStmt(node);
      break;
    case NODE_EXPR:
      printExpr(node);
      break;
    default:
      printf("(none)");
  }
}
