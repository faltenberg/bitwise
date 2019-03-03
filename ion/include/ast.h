#ifndef __AST_H__
#define __AST_H__


/**
 * Abstract Syntax Tree
 * ====================
 *
 * Source code is parsed into an AST for further introspection. The core of the syntax tree is
 * `struct ASTNode` which unites all the various information.
 *
 *
 * `struct ASTNode`
 * ----------------
 *
 * `ASTNode`s are differentiated between declarations, statements and expression in respect to the
 * `NodeKind kind` field. The safely accessible fields of `ASTNode` are further distinguished by
 * `DeclKind`, `StmtKind` and`ExprKind`, respectively. Since `ASTNode` uses a lot of unions, it is
 * only clear from the documentation which fields are safely accessible. For instance the field
 * `type` holds the declaration `Type` if and only if `kind` is set to `NODE_DECL`, while the field
 * `condition` holds the conditional expression if `kind` is set to `NODE_STMT` and `stmtKind` is
 * set to either `STMT_IF`, `STMT_WHILE`, `STMT_DOWHILE`, `STMT_FOR` or `STMT_SWITCH`. Accessing
 * wrong fields migth cause undefined behavior as not all fields are* initialized to meaningful
 * data and thus will contain garbage.
 *
 * `ASTNode` looks as follows for the various `NodeKind`s:
 *
 * ```c
 * struct ASTNode {
 *   NodeKind kind = NODE_NONE;
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_DECL;
 *   DeclKind declKind = DECL_VAR | DECL_CONST;
 *   char*    name;
 *   Type*    type;
 *   ASTNode* initValue;  // will be of kind=NODE_EXPR
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_DECL;
 *   DeclKind declKind = DECL_PARAM | DECL_FIELD;
 *   char* name;
 *   Type* type;
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_DECL;
 *   DeclKind declKind = DECL_FUNC;
 *   char*          name;
 *   Type*          returnType;
 *   SBUF(ASTNode*) parameters;  // will be of kind=NODE_DECL,DECL_PARAM
 *   ASTNode*       funcBlock;   // will be of kind=NODE_STMT,STMT_BLOCK
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_DECL;
 *   DeclKind declKind = DECL_STRUCT;
 *   char*          name;
 *   SBUF(ASTNode*) fields;  // will be of kind=NODE_DECL,DECL_FIELD
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_STMT;
 *   StmtKind stmtKind = STMT_BLOCK;
 *   SBUF(ASTNode*) statements;  // will be of kind=NODE_STMT
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_STMT;
 *   StmtKind stmtKind = STMT_BREAK | STMT_CONTINUE;
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_STMT;
 *   StmtKind stmtKind = STMT_EXPR | STMT_RETURN;
 *   ASTNode* expression;  // will be of kind=NODE_EXPR
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_STMT;
 *   StmtKind stmtKind = STMT_ASSIGN;
 *   ASTNode* lvalue;      // will be of kind=NODE_EXPR
 *   ASTNode* expression;  // will be of kind=NODE_EXPR
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_STMT;
 *   StmtKind stmtKind = STMT_ASSIGN;
 *   ASTNode* lvalue;      // will be of kind=NODE_EXPR
 *   ASTNode* expression;  // will be of kind=NODE_EXPR
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_STMT;
 *   StmtKind stmtKind = STMT_IF;
 *   ASTNode* condition;  // will be of kind=NODE_EXPR
 *   ASTNode* ifBlock;    // will be of kind=NODE_STMT,STMT_BLOCK
 *   ASTNode* elseBlock;  // will be of kind=NODE_STMT,STMT_ELSE|STMT_IF
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_STMT;
 *   StmtKind stmtKind = STMT_ELSE;
 *   SBUF(ASTNode*) statements;  // will be of kind=NODE_STMT
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_STMT;
 *   StmtKind stmtKind = STMT_WHILE | STMT_DOWHILE;
 *   ASTNode* condition;  // will be of kind=NODE_EXPR
 *   ASTNode* loopBody;   // will be of kind=NODE_STMT,STMT_BLOCK
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_STMT;
 *   StmtKind stmtKind = STMT_FOR;
 *   ASTNode* initDecl;   // will be of kind=NODE_DECL,DECL_VAR
 *   ASTNode* condition;  // will be of kind=NODE_EXPR
 *   ASTNode* loopBody;   // will be of kind=NODE_STMT,STMT_BLOCK
 *   ASTNode* postExpr;   // will be of kind=NODE_EXPR
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_EXPR;
 *   StmtKind stmtKind = EXPR_INT;
 *   uint64_t intValue;
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_EXPR;
 *   StmtKind stmtKind = EXPR_BOOL;
 *   bool boolValue;
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_EXPR;
 *   StmtKind stmtKind = EXPR_NAME;
 *   char* nameLiteral;
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_EXPR;
 *   StmtKind stmtKind = EXPR_UNARY;
 *   ASTNode* operand;  // will be of kind=NODE_EXPR
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_EXPR;
 *   StmtKind stmtKind = EXPR_BINARY;
 *   ASTNode* left;   // will be of kind=NODE_EXPR
 *   ASTNode* right;  // will be of kind=NODE_EXPR
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_EXPR;
 *   StmtKind stmtKind = EXPR_CALL;
 *   ASTNode*       operand;    // will be of kind=NODE_EXPR
 *   SBUF(ASTNode*) arguments;  // will be of kind=NODE_EXPR
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_EXPR;
 *   StmtKind stmtKind = EXPR_INDEX;
 *   ASTNode* operand;  // will be of kind=NODE_EXPR
 *   ASTNode* index;    // will be of kind=NODE_EXPR
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_EXPR;
 *   StmtKind stmtKind = EXPR_FIELD;
 *   ASTNode* operand;    // will be of kind=NODE_EXPR
 *   char*    fieldName;
 * };
 * ```
 *
 *
 * `struct Type`
 * -------------
 *
 * `Type` holds informations on declared types. Similar to `ASTNode` it uses unions to unite
 * various different type kinds in one single unit. `Type` looks as follows for the various
 * `TypeKind`s:
 *
 * ```c
 * struct Type {
 *   TypeKind kind = TYPE_NONE;
 * };
 *
 * struct Type {
 *   TypeKind kind = TYPE_PRIMITIVE;
 *   char* name;
 * };
 *
 * struct Type {
 *   TypeKind kind = TYPE_FUNC;
 *   SBUF(Type*) paramTypes;
 *   Type*       returnType;
 * };
 *
 * struct Type {
 *   TypeKind kind = TYPE_ARRAY;
 *   Type* type;
 *   Type* indexType;
 * };
 *
 * struct Type {
 *   TypeKind kind = TYPE_POINTER;
 *   Type* refType;
 * };
 *
 * struct Type {
 *   TypeKind kind = TYPE_STRUCT;
 *   char*       name;
 *   SBUF(Type*) fieldTypes;  // will be of kind=TYPE_FIELD
 * };
 *
 * struct Type {
 *   TypeKind kind = TYPE_FIELD;
 *   char* name;
 *   Type* type;
 * };
 * ```
 */


#include <stdint.h>
#include <stdbool.h>

#include "sbuffer.h"


typedef struct Type      Type;
typedef struct ASTNode   ASTNode;
typedef enum   TypeKind  TypeKind;
typedef enum   NodeKind  NodeKind;
typedef enum   DeclKind  DeclKind;
typedef enum   StmtKind  StmtKind;
typedef enum   ExprKind  ExprKind;


enum NodeKind {
  NODE_NONE,
  NODE_DECL,
  NODE_STMT,
  NODE_EXPR,
};

enum DeclKind {
  DECL_NONE,
  DECL_VAR,
  DECL_CONST,
  DECL_FUNC,
  DECL_PARAM,
  DECL_STRUCT,
  DECL_FIELD,
};

enum StmtKind {
  STMT_NONE,
  STMT_BLOCK,
  STMT_EXPR,
  STMT_ASSIGN,
  STMT_CONTINUE,
  STMT_BREAK,
  STMT_RETURN,
  STMT_IF,
  STMT_ELSE,
  STMT_WHILE,
  STMT_DOWHILE,
  STMT_FOR,
  STMT_SWITCH,
  STMT_CASE,
};

enum ExprKind {
  EXPR_NONE,
  EXPR_INT,
  EXPR_BOOL,
  EXPR_NAME,
  EXPR_UNARY,
  EXPR_BINARY,
  EXPR_CALL,
  EXPR_INDEX,
  EXPR_FIELD,
};

enum TypeKind {
  TYPE_NONE,
  TYPE_PRIMITIVE,
  TYPE_FUNC,
  TYPE_ARRAY,
  TYPE_POINTER,
  TYPE_STRUCT,
  TYPE_FIELD,
};

struct Type {
  TypeKind kind;
  char* name;
  union {
    Type* type;
    Type* refType;
    Type* returnType;
  };
  union {
    Type*       indexType;
    SBUF(Type*) fieldTypes;
    SBUF(Type*) paramTypes;
  };
};


struct ASTNode {
  NodeKind kind;

  union {

    struct {
      DeclKind declKind;
      char* name;
      union {
        Type* type;
        Type* returnType;
      };
      union {
        ASTNode*       initValue;
        SBUF(ASTNode*) fields;
        struct {
          SBUF(ASTNode*) parameters;
          ASTNode*       funcBlock;
        };
      };
    };

    struct {
      StmtKind stmtKind;
      union {
        ASTNode* expression;
        ASTNode* condition;
      };
      union {
        SBUF(ASTNode*) statements;
        SBUF(ASTNode*) cases;
        ASTNode*       lvalue;
        struct {
          ASTNode* ifBlock;
          ASTNode* elseBlock;
        };
        struct {
          ASTNode* initDecl;
          ASTNode* loopBody;
          ASTNode* postExpr;
        };
      };
    };

    struct {
      ExprKind exprKind;
      union {
        uint64_t intValue;
        bool     boolValue;
        char*    nameLiteral;
        struct {
          ASTNode* left;
          ASTNode* right;
        };
        struct {
          ASTNode* operand;
          union {
            SBUF(ASTNode*) arguments;
            ASTNode*       index;
            char*          fieldName;
          };
        };
      };
    };

  };
};


#endif  // __AST_H__
