#include "parser.h"

#include <stdlib.h>
#include <stdio.h>

#include "lexer.h"
#include "token.h"


static ASTNode* createNode(ASTKind kind) {
  ASTNode* node = (ASTNode*) malloc(sizeof(ASTNode));
  node->kind = kind;
  return node;
}


static ASTNode* createErrorNode(ASTNode* errorNode, const char* message) {
  ASTNode* node = createNode(AST_ERROR);
  node->errorNode = errorNode;
  node->errorMsg = message;
  return node;
}


static ASTNode* createExprNode(ASTExprKind kind) {
  ASTNode* node = createNode(AST_EXPR);
  node->exprKind = kind;
  return node;
}


static ASTNode* createExprInt(int value) {
  ASTNode* node = createExprNode(EXPR_INT);
  node->value = value;
  return node;
}


static ASTNode* createExprName(const char* name) {
  ASTNode* node = createExprNode(EXPR_NAME);
  node->name = name;
  return node;
}


static ASTNode* createExprUnop(const char* op, ASTNode* rhs) {
  ASTNode* node = createExprNode(EXPR_UNOP);
  node->op = op;
  node->rhs = rhs;
  return node;
}


static ASTNode* createExprBinop(ASTNode* lhs, const char* op, ASTNode* rhs) {
  ASTNode* node = createExprNode(EXPR_BINOP);
  node->lhs = lhs;
  node->op = op;
  node->rhs = rhs;
  return node;
}


static ASTNode* createExprParen(ASTNode* expr) {
  ASTNode* node = createExprNode(EXPR_PAREN);
  node->expr = expr;
  return node;
}


static ASTNode* createExprField(ASTNode* lvalue, const char* field) {
  ASTNode* node = createExprNode(EXPR_FIELD);
  node->lvalue = lvalue;
  node->field = field;
  return node;
}


/*
expr = term
     | term "+" expr

term = factor
     | factor "*" term

factor = INT
       | NAME
       | "(" expr ")"
*/
static ASTNode* parseExpr(Lexer* lexer);
static ASTNode* parseTerm(Lexer* lexer);
static ASTNode* parseFactor(Lexer* lexer);


static ASTNode* parseFactor(Lexer* lexer) {
  Token next = peekToken(lexer);

  if (next.kind == TOKEN_NAME) {
    nextToken(lexer);
    return createExprName(next.content);
  }

  if (next.kind == TOKEN_INT) {
    nextToken(lexer);
    return createExprInt(atoi(next.content));
  }

  if (next.kind == TOKEN_SEP && next.content == "(") {
    nextToken(lexer);
    ASTNode* expr = parseExpr(lexer);
    next = peekToken(lexer);
    if (next.kind == TOKEN_SEP && next.content == ")") {
      nextToken(lexer);
      return createExprParen(expr);
    } else {
      return createErrorNode(expr, "missing matching \")\"");
    }
  }

  return createErrorNode(createExprNode(EXPR_NONE), "cannot parse expression");
}


static ASTNode* parseTerm(Lexer* lexer) {
  ASTNode* lhs = parseFactor(lexer);

  Token next = peekToken(lexer);

  if (next.kind == TOKEN_OP && next.content == "*") {
    nextToken(lexer);
    ASTNode* rhs = parseTerm(lexer);
    return createExprBinop(lhs, next.content, rhs);
  }

  return lhs;
}


static ASTNode* parseExpr(Lexer* lexer) {
  ASTNode* lhs = parseTerm(lexer);

  Token next = peekToken(lexer);

  if (next.kind == TOKEN_OP && next.content == "+") {
    nextToken(lexer);
    ASTNode* rhs = parseExpr(lexer);
    return createExprBinop(lhs, next.content, rhs);
  }

  return lhs;
}


ASTNode* parse(Source src) {
  printf("parse %s: \"%s\"\n", src.fileName.chars, src.content.chars);

  Lexer lexer = newLexer(src);
  ASTNode* expr = parseExpr(&lexer);
  if (peekToken(&lexer).kind != TOKEN_EOF) {
    return createErrorNode(expr, "invalid expr following");
  } else {
    return expr;
  }
}


void deleteNode(ASTNode* node) {
  free(node);
}


static void printAST__(const ASTNode* node) {
  switch (node->kind) {
    case AST_NONE:
      printf("(none)");
      break;

    case AST_ERROR:
      printf("(error \"%s\" in ", node->errorMsg);
      printAST__(node->errorNode);
      printf(")");
      break;

    case AST_EXPR:
      switch (node->exprKind) {
        case EXPR_NONE:
          printf("(none)");
          break;
        case EXPR_INT:
          printf("%d", node->value);
          break;
        case EXPR_NAME:
          printf("%s", node->name);
          break;
        case EXPR_UNOP:
          printf("(%s ", node->op);
          printAST__(node->rhs);
          printf(")");
          break;
        case EXPR_BINOP:
          printf("(%s ", node->op);
          printAST__(node->lhs);
          printf(" ");
          printAST__(node->rhs);
          printf(")");
          break;
        case EXPR_PAREN:
          printf("(");
          printAST__(node->expr);
          printf(")");
          break;
        case EXPR_FIELD:
          printf("(field ");
          printAST__(node->lvalue);
          printf(" %s)", node->field);
          break;
        default:
          printf("(unknown)");
      }
      break;

    default:
      printf("(unknown)");
      break;
  }
}


void printAST(const ASTNode* node) {
  printAST__(node);
  printf("\n");
}
