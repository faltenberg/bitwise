#ifndef __PARSER_H__
#define __PARSER_H__


#include "source.h"


typedef enum ASTKind {
  AST_NONE,
  AST_ERROR,
  AST_EXPR,
} ASTKind;


typedef enum ASTExprKind {
  EXPR_NONE,
  EXPR_INT,
  EXPR_NAME,
  EXPR_UNOP,
  EXPR_BINOP,
  EXPR_PAREN,
  EXPR_FIELD,
} ASTExprKind;


typedef struct ASTNode {
  ASTKind kind;
  union {

    struct {
      struct ASTNode* errorNode;
      const char*     errorMsg;
    };

    struct {
      ASTExprKind exprKind;
      union {
        int             value;
        const char*     name;
        struct ASTNode* expr;
        struct {
          const char*     op;
          struct ASTNode* lhs;
          struct ASTNode* rhs;
        };
        struct {
          struct ASTNode* lvalue;
          const char*     field;
        };
      };
    };

  };
} ASTNode;


ASTNode* parse(Source src);

void deleteNode(ASTNode* node);

void printAST(const ASTNode* node);


#endif  // __PARSER_H__