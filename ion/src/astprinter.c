#include "astprinter.h"

#include <stdio.h>


static void printExpr(const ASTNode* expr);
static void printBlock(const ASTNode* block);
static void printStmt(const ASTNode* stmt);
static void printDecl(const ASTNode* decl);

static int intend = 0;

static void newLine() {
  printf("\n");
  for (int i = 0; i < intend; i++) {
     printf(".  ");
  }
}


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
    case TYPE_NAME:
      printf("%s", type->name);
      break;
    case TYPE_FUNC:
      printf("(func (");
      for (int i = 0; i < bufLength(type->paramTypes); i++) {
        printType(type->paramTypes[i]);
        if (i < bufLength(type->paramTypes) - 1) {
          printf(" ");
        }
      }
      printf(") ");
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
      printf("(struct %s", type->name);
      for (int i = 0; i < bufLength(type->fieldTypes); i++) {
        printf(" ");
        printType(type->fieldTypes[i]);
      }
      printf(")");
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
    case NODE_NONE:
      printf("()");
      break;
    case NODE_DECL:
      printDecl(node);
      break;
    case NODE_STMT:
      printStmt(node);
      break;
    case NODE_EXPR:
      printExpr(node);
      break;
    case NODE_ERROR:
      printf("(error \"%s\")", node->errorMessage);
      break;
  }
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
    case TOKEN_OP_CMP_LT:
      return "<";
    case TOKEN_OP_CMP_LEQ:
      return "<=";
    case TOKEN_OP_CMP_GT:
      return ">";
    case TOKEN_OP_CMP_GEQ:
      return ">=";
    case TOKEN_OP_CMP_EQ:
      return "==";
    case TOKEN_OP_CMP_NEQ:
      return "!=";
    case TOKEN_OP_LSL:
      return "<<";
    case TOKEN_OP_LSR:
      return ">>";
    case TOKEN_OP_ASR:
      return ">>>";
    default:
      return "op<>";
  }
};


static void printExpr(const ASTNode* expr) {
  switch (expr->exprKind) {
    case EXPR_NONE:
      printf("()");
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


static void printBlock(const ASTNode* block) {
  printf("(");
  ++intend;

  for (int i = 0; i < bufLength(block->statements); i++) {
    newLine();
    printStmt(block->statements[i]);
  }

  --intend;
  newLine();
  printf(")");
}


static void printStmt(const ASTNode* stmt) {
  switch (stmt->stmtKind) {
    case STMT_NONE:
      printf("()");
      break;
    case STMT_BLOCK:
      printBlock(stmt);
      break;
    case STMT_EXPR:
      printf("(");
      printExpr(stmt->expression);
      printf(")");
      break;
    case STMT_RETURN:
      printf("(return ");
      printExpr(stmt->expression);
      printf(")");
      break;
    case STMT_ASSIGN:
      printf("(= ");
      printExpr(stmt->lvalue);
      printf(" ");
      printExpr(stmt->expression);
      printf(")");
      break;
    case STMT_BREAK:
      printf("(break)");
      break;
    case STMT_CONTINUE:
      printf("(continue)");
      break;
    case STMT_IF:
      printf("(if ");
      printExpr(stmt->condition);
      ++intend;
      newLine();
      printStmt(stmt->ifBlock);
      newLine();
      printStmt(stmt->elseBlock);
      --intend;
      newLine();
      printf(")");
      break;
    case STMT_ELSEIF:
      printf("(else if ");
      printExpr(stmt->condition);
      ++intend;
      newLine();
      printStmt(stmt->ifBlock);
      newLine();
      printStmt(stmt->elseBlock);
      --intend;
      newLine();
      printf(")");
      break;
    case STMT_ELSE:
      printf("(else");
      ++intend;
      newLine();
      printBlock(stmt);
      --intend;
      newLine();
      printf(")");
      break;
    case STMT_WHILE:
      printf("(while ");
      printExpr(stmt->condition);
      ++intend;
      newLine();
      printBlock(stmt->loopBody);
      --intend;
      newLine();
      printf(")");
      break;
    case STMT_DOWHILE:
      printf("(do-while ");
      printExpr(stmt->condition);
      ++intend;
      newLine();
      printBlock(stmt->loopBody);
      --intend;
      newLine();
      printf(")");
      break;
    case STMT_FOR:
      printf("(for ");
      printDecl(stmt->initDecl);
      printf(" ");
      printExpr(stmt->condition);
      printf(" ");
      printExpr(stmt->postExpr);
      ++intend;
      newLine();
      printBlock(stmt->loopBody);
      --intend;
      newLine();
      printf(")");
      break;
    case STMT_SWITCH:
      printf("(switch ");
      printExpr(stmt->expression);
      ++intend;
      for (int i = 0; i < bufLength(stmt->cases); i++) {
        newLine();
        printStmt(stmt->cases[i]);
      }
      --intend;
      newLine();
      printf(")");
      break;
    case STMT_CASE:
      printf("(case ");
      printExpr(stmt->condition);
      ++intend;
      newLine();
      printBlock(stmt->caseBlock);
      --intend;
      newLine();
      printf(")");
  }
}


static void printDecl(const ASTNode* decl) {
  switch (decl->declKind) {
    case DECL_NONE:
      break;
    case DECL_VAR:
      printf("(var %s ", decl->name);
      printType(decl->type);
      printf(" ");
      printExpr(decl->initValue);
      printf(")");
      break;
    case DECL_CONST:
      printf("(const %s ", decl->name);
      printType(decl->type);
      printf(" ");
      printExpr(decl->initValue);
      printf(")");
      break;
    case DECL_STRUCT:
      printf("(struct %s ", decl->name);
      ++intend;
      newLine();
      for (int i = 0; i < bufLength(decl->fields); i++) {
        printDecl(decl->fields[i]);
        newLine();
      }
      --intend;
      newLine();
      printf(")");
      break;
    case DECL_FUNC:
      printf("(func %s ", decl->name);
      printf("(");
      for (int i = 0; i < bufLength(decl->parameters); i++) {
        printDecl(decl->parameters[i]);
        if (i < bufLength(decl->parameters) - 1) {
          printf(" ");
        }
      }
      printf(") ");
      printType(decl->returnType);
      ++intend;
      newLine();
      printStmt(decl->funcBlock);
      --intend;
      newLine();
      printf(")");
      break;
    case DECL_FIELD:
    case DECL_PARAM:
      printf("(%s ", decl->name);
      printType(decl->type);
      printf(")");
      break;
  }
}
