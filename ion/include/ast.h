#ifndef __AST_H__
#define __AST_H__


/**
 * Abstract Syntax Tree
 * ====================
 *
 * Source code is parsed into an AST for further introspection. The core of the syntax tree is
 * `struct ASTNode` which unites all the various information. `struct Type` holds information about
 * some type. Like `ASTNode` it uses unions and the fields are meant to be accessed as described
 * later in the documentation.
 *
 *
 * Node Creation
 * -------------
 *
 * The API defines a bunch of factory functions to create various `ASTNode`s of different kinds.
 * This simplifies the node creation a lot. Those functions will allocate new nodes on the heap
 * and should be deleted with the provided `deleteNode()` function.
 *
 * ```c
 * #include "ast.h"
 * #include <assert.h>
 *
 * int main() {
 *   ASTNode* node = createEmptyNode();
 *   assert(node != NULL);
 *   assert(node->kind == NODE_NONE);
 *   deleteNode(&node);
 *   assert(node == NULL);
 * }
 * ```
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
 *   NodeKind kind = NODE_ERROR;
 *   char*    errorMessage;
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
 *   DeclKind declKind = DECL_STRUCT;
 *   char*          name;
 *   SBUF(ASTNode*) fields;  // will be of kind=NODE_DECL,DECL_FIELD
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
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_STMT;
 *   StmtKind stmtKind = STMT_NONE;
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
 *   StmtKind stmtKind = STMT_EXPR;
 *   ASTNode* expression;  // will be of kind=NODE_EXPR
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_STMT;
 *   StmtKind stmtKind = STMT_RETURN;
 *   ASTNode* expression;  // will be of kind=NODE_EXPR|NODE_NONE
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
 *   StmtKind stmtKind = STMT_IF|STMT_ELSEIF;
 *   ASTNode* condition;  // will be of kind=NODE_EXPR
 *   ASTNode* ifBlock;    // will be of kind=NODE_STMT,STMT_BLOCK
 *   ASTNode* elseBlock;  // will be of kind=NODE_STMT,STMT_ELSE|STMT_ELSEIF
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
 *   NodeKind kind     = NODE_STMT;
 *   StmtKind stmtKind = STMT_BREAK | STMT_CONTINUE;
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_STMT;
 *   StmtKind stmtKind = STMT_SWITCH;
 *   ASTNode*       expression;  // will be of kind=NODE_EXPR
 *   SBUF(ASTNode*) cases;       // will be of kind=NODE_STMT,STMT_CASE|STMT_ELSE
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_STMT;
 *   StmtKind stmtKind = STMT_CASE;
 *   ASTNode* condition;  // will be of kind=NODE_EXPR
 *   ASTNode* caseBlock;  // will be of kind=NODE_STMT,STMT_BLOCK
 * };
 *
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
 *   ASTNode*  operand;  // will be of kind=NODE_EXPR
 *   TokenKind unop;
 * };
 *
 * struct ASTNode {
 *   NodeKind kind     = NODE_EXPR;
 *   StmtKind stmtKind = EXPR_BINARY;
 *   ASTNode*  left;   // will be of kind=NODE_EXPR
 *   ASTNode*  right;  // will be of kind=NODE_EXPR
 *   TokenKind binop;
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
 *   TypeKind kind = TYPE_NAME;
 *   char* name;
 * };
 *
 * struct Type {
 *   TypeKind kind = TYPE_ARRAY;
 *   Type*    baseType;
 *   uint64_t arraySize;
 * };
 *
 * struct Type {
 *   TypeKind kind = TYPE_POINTER;
 *   Type* baseType;
 * };
 *
 * struct Type {
 *   TypeKind kind = TYPE_STRUCT;
 *   char*       name;
 *   SBUF(Type*) fieldTypes;
 * };
 *
 * struct Type {
 *   TypeKind kind = TYPE_FUNC;
 *   SBUF(Type*) paramTypes;
 *   Type*       returnType;
 * };
 * ```
 */


#include <stdint.h>
#include <stdbool.h>

#include "sbuffer.h"
#include "token.h"


typedef struct Type    Type;
typedef struct ASTNode ASTNode;


typedef enum NodeKind {
  NODE_NONE,
  NODE_DECL,
  NODE_STMT,
  NODE_EXPR,
  NODE_ERROR,
} NodeKind;


typedef enum DeclKind {
  DECL_NONE,
  DECL_VAR,
  DECL_CONST,
  DECL_STRUCT,
  DECL_FIELD,
  DECL_FUNC,
  DECL_PARAM,
} DeclKind;


typedef enum StmtKind {
  STMT_NONE,
  STMT_BLOCK,
  STMT_EXPR,
  STMT_RETURN,
  STMT_ASSIGN,
  STMT_BREAK,
  STMT_CONTINUE,
  STMT_IF,
  STMT_ELSEIF,
  STMT_ELSE,
  STMT_WHILE,
  STMT_DOWHILE,
  STMT_FOR,
  STMT_SWITCH,
  STMT_CASE,
} StmtKind;


typedef enum ExprKind {
  EXPR_NONE,
  EXPR_INT,
  EXPR_BOOL,
  EXPR_NAME,
  EXPR_UNARY,
  EXPR_BINARY,
  EXPR_CALL,
  EXPR_INDEX,
  EXPR_FIELD,
} ExprKind;


typedef enum TypeKind {
  TYPE_NONE,
  TYPE_NAME,
  TYPE_FUNC,
  TYPE_ARRAY,
  TYPE_POINTER,
  TYPE_STRUCT,
} TypeKind;


struct Type {
  TypeKind kind;
  union {
    struct {
      char*       name;
      SBUF(Type*) fieldTypes;
    };
    struct {
      SBUF(Type*) paramTypes;
      Type*       returnType;
    };
    struct {
      Type*    baseType;
      uint64_t arraySize;
    };
  };
};


struct ASTNode {
  NodeKind kind;

  union {
    struct {
      char* errorMessage;
    };

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
        ASTNode*       caseBlock;
        struct {
          ASTNode* ifBlock;
          ASTNode* elseBlock;
        };
        struct {
          ASTNode* loopBody;
          ASTNode* initDecl;
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
          ASTNode*  left;
          ASTNode*  right;
          TokenKind binop;
        };
        struct {
          ASTNode* operand;
          union {
            TokenKind      unop;
            SBUF(ASTNode*) arguments;
            ASTNode*       index;
            char*          fieldName;
          };
        };
      };
    };

  };
};


void deleteNode(ASTNode* node);

ASTNode* createEmptyNode();

ASTNode* createErrorNode(char* errorMessage);


ASTNode* createExprInt(uint64_t value);

ASTNode* createExprBool(bool value);

ASTNode* createExprName(char* name);

ASTNode* createExprUnary(TokenKind op, ASTNode* operand);

ASTNode* createExprBinary(ASTNode* left, TokenKind op, ASTNode* right);

ASTNode* createExprCall(ASTNode* operand, int argCount, ...);

ASTNode* createExprIndex(ASTNode* operand, ASTNode* index);

ASTNode* createExprField(ASTNode* operand, char* fieldName);


ASTNode* createStmtEmpty();

ASTNode* createStmtBlock(int stmtCount, ...);

ASTNode* createStmtExpr(ASTNode* expression);

ASTNode* createStmtReturn(ASTNode* expression);

ASTNode* createStmtAssign(ASTNode* lvalue, ASTNode* expression);

ASTNode* createStmtIf(ASTNode* condition, ASTNode* ifBlock, ASTNode* elseBlock);

ASTNode* createStmtElseIf(ASTNode* condition, ASTNode* ifBlock, ASTNode* elseBlock);

ASTNode* createStmtElse(int stmtCount, ...);

ASTNode* createStmtWhile(ASTNode* condition, ASTNode* loopBody);

ASTNode* createStmtDoWhile(ASTNode* condition, ASTNode* loopBody);

ASTNode* createStmtFor(ASTNode* initDecl, ASTNode* condition, ASTNode* postExpr, ASTNode* loopBody);

ASTNode* createStmtBreak();

ASTNode* createStmtContinue();

ASTNode* createStmtSwitch(ASTNode* expression, int caseCount, ...);

ASTNode* createStmtCase(ASTNode* condition, ASTNode* caseBlock);


ASTNode* createDeclVar(char* name, Type* type, ASTNode* initValue);

ASTNode* createDeclConst(char* name, Type* type, ASTNode* initValue);

ASTNode* createDeclStruct(char* name, int fieldCount, ...);

ASTNode* createDeclField(char* name, Type* type);

ASTNode* createDeclFunc(char* name, ASTNode* funcBlock, Type* returnType, int paramCount, ...);

ASTNode* createDeclParam(char* name, Type* type);


void deleteType(Type* type);

Type* createTypeNone();

Type* createTypeName(char* name);

Type* createTypePointer(Type* baseType);

Type* createTypeArray(Type* baseType, uint64_t size);

Type* createTypeFunc(Type* returnType, int paramCount, ...);

Type* createTypeStruct(char* name, int fieldCount, ...);


#endif  // __AST_H__
