#include "ast.h"

#include <stdarg.h>


void deleteNode(ASTNode* *node) {
  if (*node == NULL) {
    return;
  }

  switch ((*node)->kind) {
    case NODE_DECL:
      switch ((*node)->declKind) {
        case DECL_VAR:
        case DECL_CONST:
          deleteNode(&(*node)->initValue);
          break;
        case DECL_FUNC:
          for (int i = 0; i < bufLength((*node)->parameters); i++) {
            deleteNode(&(*node)->parameters[i]);
          }
          bufFree((*node)->parameters);
          deleteNode(&(*node)->funcBlock);
          break;
        case DECL_STRUCT:
          for (int i = 0; i < bufLength((*node)->fields); i++) {
            deleteNode(&(*node)->fields[i]);
          }
          bufFree((*node)->fields);
          break;
        default: ;
      }
      break;

    case NODE_STMT:
      switch ((*node)->stmtKind) {
        case STMT_BLOCK:
          for (int i = 0; i < bufLength((*node)->statements); i++) {
            deleteNode(&(*node)->statements[i]);
          }
          bufFree((*node)->statements);
          break;
        case STMT_EXPR:
        case STMT_RETURN:
          deleteNode(&(*node)->expression);
          break;
        case STMT_ASSIGN:
          deleteNode(&(*node)->lvalue);
          deleteNode(&(*node)->expression);
          break;
        case STMT_IF:
          deleteNode(&(*node)->condition);
          deleteNode(&(*node)->ifBlock);
          deleteNode(&(*node)->elseBlock);
          break;
        case STMT_ELSE:
          for (int i = 0; i < bufLength((*node)->statements); i++) {
            deleteNode(&(*node)->statements[i]);
          }
          bufFree((*node)->statements);
          break;
        case STMT_FOR:
          deleteNode(&(*node)->initDecl);
          deleteNode(&(*node)->postExpr);
        case STMT_WHILE:
        case STMT_DOWHILE:
          deleteNode(&(*node)->condition);
          deleteNode(&(*node)->loopBody);
          break;
        case STMT_SWITCH:
          deleteNode(&(*node)->expression);
          for (int i = 0; i < bufLength((*node)->cases); i++) {
            deleteNode(&(*node)->cases[i]);
          }
          bufFree((*node)->cases);
          break;
        case STMT_CASE:
          deleteNode(&(*node)->condition);
          deleteNode(&(*node)->caseBlock);
          break;
        default: ;
      }
      break;

    case NODE_EXPR:
      switch ((*node)->exprKind) {
        case EXPR_BINARY:
          deleteNode(&(*node)->left);
          deleteNode(&(*node)->right);
          break;
        case EXPR_CALL:
          deleteNode(&(*node)->operand);
          for (int i = 0; i < bufLength((*node)->arguments); i++) {
            deleteNode(&(*node)->arguments[i]);
          }
          bufFree((*node)->arguments);
          break;
        case EXPR_INDEX:
          deleteNode(&(*node)->index);
        case EXPR_UNARY:
        case EXPR_FIELD:
          deleteNode(&(*node)->operand);
          break;
        default: ;
      }
      break;

    default: ;
  }

  free(*node);
  *node = NULL;
}


static ASTNode* createNode(NodeKind kind) {
  ASTNode* node = (ASTNode*) calloc(1, sizeof(ASTNode));
  node->kind = kind;
  return node;
}


static ASTNode* createExprNode(ExprKind kind) {
  ASTNode* node = createNode(NODE_EXPR);
  node->exprKind = kind;
  return node;
}


ASTNode* createEmptyNode() {
  return createNode(NODE_NONE);
}


ASTNode* createExprInt(uint64_t value) {
  ASTNode* node = createExprNode(EXPR_INT);
  node->intValue = value;
  return node;
}


ASTNode* createExprBool(bool value) {
  ASTNode* node = createExprNode(EXPR_BOOL);
  node->boolValue = value;
  return node;
}


ASTNode* createExprName(char* name) {
  ASTNode* node = createExprNode(EXPR_NAME);
  node->nameLiteral = name;
  return node;
}


ASTNode* createExprUnary(TokenKind op, ASTNode* operand) {
  ASTNode* node = createExprNode(EXPR_UNARY);
  node->unop = op;
  node->operand = operand;
  return node;
}


ASTNode* createExprBinary(ASTNode* left, TokenKind op, ASTNode* right) {
  ASTNode* node = createExprNode(EXPR_BINARY);
  node->binop = op;
  node->left = left;
  node->right = right;
  return node;
}


ASTNode* createExprCall(ASTNode* operand, int argCount, ...) {
  ASTNode* node = createExprNode(EXPR_CALL);
  node->operand = operand;
  node->arguments = NULL;
  bufFit(node->arguments, argCount);

  va_list args;
  va_start(args, argCount);
  for (int i = 0; i < argCount; i++) {
    ASTNode* arg = va_arg(args, ASTNode*);
    bufPush(node->arguments, arg);
  }
  va_end(args);

  return node;
}


ASTNode* createExprIndex(ASTNode* operand, ASTNode* index) {
  ASTNode* node = createExprNode(EXPR_INDEX);
  node->operand = operand;
  node->index = index;
  return node;
}


ASTNode* createExprField(ASTNode* operand, char* fieldName) {
  ASTNode* node = createExprNode(EXPR_FIELD);
  node->operand = operand;
  node->fieldName = fieldName;
  return node;
}
