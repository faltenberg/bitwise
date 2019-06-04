#ifndef __AST_H__
#define __AST_H__


/**
 * Abstract Syntax Tree
 * ====================
 *
 * ...
 *
 *
 * Example
 * -------
 *
 * ```c {.line-numbers}
 * #include "ast.h"
 *
 * int main() {
 *   // ...
 * }
 * ```
 */


#include "sbuffer.h"
#include "source.h"
#include "number.h"


typedef enum ExprKind {
  EXPR_NONE,
  EXPR_INT,
  EXPR_NAME,
  EXPR_PAREN,
  EXPR_UNOP,
  EXPR_BINOP,
} ExprKind;


const char* strExprKind(ExprKind kind);


typedef struct ASTExpr {
  ExprKind kind;
  union {
    Number            value;
    string            name;
    struct ASTNode*   expr;
    struct {
      struct ASTNode* lhs;
      string          op;
      struct ASTNode* rhs;
    };
  };
} ASTExpr;


typedef enum ASTKind {
  AST_NONE,
  AST_ERROR,
  AST_EXPR,
} ASTKind;


const char* strASTKind(ASTKind kind);


typedef struct ASTNode {
  ASTKind      kind;
  SBUF(string) messages;
  union {
    struct ASTNode* faultyNode;
    ASTExpr         expr;
  };
} ASTNode;


void deleteNode(ASTNode* node);


#endif  // __PARSER_H__
