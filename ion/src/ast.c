#include "ast.h"

#include <stdarg.h>
#include <stdio.h>


static Type* createType(TypeKind kind) {
  Type* type = (Type*) calloc(1, sizeof(Type));
  type->kind = kind;
  return type;
}


Type* createTypeNone() {
  return createType(TYPE_NONE);
}


Type* createTypeName(char* name) {
  Type* type = createType(TYPE_NAME);
  type->name = name;
  return type;
}


Type* createTypePointer(Type* baseType) {
  Type* type = createType(TYPE_POINTER);
  type->baseType = baseType;
  return type;
}


Type* createTypeArray(Type* baseType, uint64_t size) {
  Type* type = createType(TYPE_ARRAY);
  type->baseType = baseType;
  type->arraySize = size;
  return type;
}


Type* createTypeFunc(Type* returnType, int paramCount, ...) {
  Type* type = createType(TYPE_FUNC);
  type->returnType = returnType;

  va_list params;
  va_start(params, paramCount);
  for (int i = 0; i < paramCount; i++) {
    Type* param = va_arg(params, Type*);
    bufPush(type->paramTypes, param);
  }
  va_end(params);

  return type;
}


Type* createTypeStruct(char* name, int fieldCount, ...) {
  Type* type = createType(TYPE_STRUCT);
  type->name = name;

  va_list fields;
  va_start(fields, fieldCount);
  for (int i = 0; i < fieldCount; i++) {
    Type* field = va_arg(fields, Type*);
    bufPush(type->fieldTypes, field);
  }
  va_end(fields);

  return type;
}


static ASTNode* createNode(NodeKind kind) {
  ASTNode* node = (ASTNode*) calloc(1, sizeof(ASTNode));
  node->kind = kind;
  return node;
}


ASTNode* createEmptyNode() {
  return createNode(NODE_NONE);
}


ASTNode* createErrorNode(char* errorMessage) {
  ASTNode* node = createNode(NODE_ERROR);
  node->errorMessage = errorMessage;
  return node;
}


static ASTNode* createExprNode(ExprKind kind) {
  ASTNode* node = createNode(NODE_EXPR);
  node->exprKind = kind;
  return node;
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


static ASTNode* createStmtNode(StmtKind kind) {
  ASTNode* node = createNode(NODE_STMT);
  node->stmtKind = kind;
  return node;
}


ASTNode* createStmtEmpty() {
  return createStmtNode(STMT_NONE);
}


ASTNode* createStmtBlock(int stmtCount, ...) {
  ASTNode* node = createStmtNode(STMT_BLOCK);
  node->statements = NULL; bufFit(node->statements, stmtCount);

  va_list stmts;
  va_start(stmts, stmtCount);
  for (int i = 0; i < stmtCount; i++) {
    ASTNode* stmt = va_arg(stmts, ASTNode*);
    bufPush(node->statements, stmt);
  }
  va_end(stmts);

  return node;
}


ASTNode* createStmtExpr(ASTNode* expression) {
  ASTNode* node = createStmtNode(STMT_EXPR);
  node->expression = expression;
  return node;
}


ASTNode* createStmtReturn(ASTNode* expression) {
  ASTNode* node = createStmtNode(STMT_RETURN);
  node->expression = expression;
  return node;
}


ASTNode* createStmtAssign(ASTNode* lvalue, ASTNode* expression) {
  ASTNode* node = createStmtNode(STMT_ASSIGN);
  node->lvalue = lvalue;
  node->opkind = TOKEN_ASSIGN;
  node->expression = expression;
  return node;
}

ASTNode* createStmtAssignOp(ASTNode* lvalue, TokenKind opkind, ASTNode* expression) {
  ASTNode* node = createStmtNode(STMT_ASSIGN);
  node->lvalue = lvalue;
  node->opkind = opkind;
  node->expression = expression;
  return node;
}


ASTNode* createStmtIf(ASTNode* condition, ASTNode* ifBlock, ASTNode* elseBlock) {
  ASTNode* node = createStmtNode(STMT_IF);
  node->condition = condition;
  node->ifBlock = ifBlock;
  node->elseBlock = elseBlock;
  return node;
}


ASTNode* createStmtElseIf(ASTNode* condition, ASTNode* ifBlock, ASTNode* elseBlock) {
  ASTNode* node = createStmtNode(STMT_ELSEIF);
  node->condition = condition;
  node->ifBlock = ifBlock;
  node->elseBlock = elseBlock;
  return node;
}


ASTNode* createStmtElse(int stmtCount, ...) {
  ASTNode* node = createStmtNode(STMT_ELSE);
  node->statements = NULL; bufFit(node->statements, stmtCount);

  va_list stmts;
  va_start(stmts, stmtCount);
  for (int i = 0; i < stmtCount; i++) {
    ASTNode* stmt = va_arg(stmts, ASTNode*);
    bufPush(node->statements, stmt);
  }
  va_end(stmts);

  return node;
}


ASTNode* createStmtWhile(ASTNode* condition, ASTNode* loopBody) {
  ASTNode* node = createStmtNode(STMT_WHILE);
  node->condition = condition;
  node->loopBody = loopBody;
  return node;
}


ASTNode* createStmtDoWhile(ASTNode* condition, ASTNode* loopBody) {
  ASTNode* node = createStmtNode(STMT_DOWHILE);
  node->condition = condition;
  node->loopBody = loopBody;
  return node;
}


ASTNode* createStmtFor(ASTNode* initDecl, ASTNode* condition, ASTNode* postStmt, ASTNode* loopBody) {
  ASTNode* node = createStmtNode(STMT_FOR);
  node->initDecl = initDecl;
  node->condition = condition;
  node->postStmt = postStmt;
  node->loopBody = loopBody;
  return node;
}


ASTNode* createStmtBreak() {
  return createStmtNode(STMT_BREAK);
}


ASTNode* createStmtContinue() {
  return createStmtNode(STMT_CONTINUE);
}


ASTNode* createStmtSwitch(ASTNode* expression, int caseCount, ...) {
  ASTNode* node = createStmtNode(STMT_SWITCH);
  node->expression = expression;

  va_list cases;
  va_start(cases, caseCount);
  for (int i = 0; i < caseCount; i++) {
    ASTNode* c = va_arg(cases, ASTNode*);
    bufPush(node->cases, c);
  }
  va_end(cases);

  return node;
}


ASTNode* createStmtCase(ASTNode* condition, ASTNode* caseBlock) {
  ASTNode* node = createStmtNode(STMT_CASE);
  node->condition = condition;
  node->caseBlock = caseBlock;
  return node;
}


static ASTNode* createDeclNode(DeclKind kind) {
  ASTNode* node = createNode(NODE_DECL);
  node->declKind = kind;
  return node;
}


ASTNode* createDeclVar(char* name, Type* type, ASTNode* initValue) {
  ASTNode* node = createDeclNode(DECL_VAR);
  node->name = name;
  node->type = type;
  node->initValue = initValue;
  return node;
}


ASTNode* createDeclConst(char* name, Type* type, ASTNode* initValue) {
  ASTNode* node = createDeclNode(DECL_CONST);
  node->name = name;
  node->type = type;
  node->initValue = initValue;
  return node;
}


ASTNode* createDeclStruct(char* name, int fieldCount, ...) {
  ASTNode* node = createDeclNode(DECL_STRUCT);
  node->name = name;

  va_list fields;
  va_start(fields, fieldCount);
  for (int i = 0; i < fieldCount; i++) {
    ASTNode* field = va_arg(fields, ASTNode*);
    bufPush(node->fields, field);
  }
  va_end(fields);

  return node;
}


ASTNode* createDeclField(char* name, Type* type) {
  ASTNode* node = createDeclNode(DECL_FIELD);
  node->name = name;
  node->type = type;
  return node;
}


ASTNode* createDeclFunc(char* name, ASTNode* funcBlock, Type* returnType, int paramCount, ...) {
  ASTNode* node = createDeclNode(DECL_FUNC);
  node->name = name;
  node->funcBlock = funcBlock;
  node->returnType = returnType;

  va_list params;
  va_start(params, paramCount);
  for (int i = 0; i < paramCount; i++) {
    ASTNode* param = va_arg(params, ASTNode*);
    bufPush(node->parameters, param);
  }
  va_end(params);

  return node;
}


ASTNode* createDeclParam(char* name, Type* type) {
  ASTNode* node = createDeclNode(DECL_PARAM);
  node->name = name;
  node->type = type;
  return node;
}


void deleteType(Type* type) {
  if (type == NULL) {
    return;
  }

  switch (type->kind) {
    case TYPE_NONE:
    case TYPE_NAME:
      break;
    case TYPE_ARRAY:
    case TYPE_POINTER:
      deleteType(type->baseType);
      break;
    case TYPE_FUNC:
      for (int i = 0; i < bufLength(type->paramTypes); i++) {
        deleteType(type->paramTypes[i]);
      }
      bufFree(type->paramTypes);
      deleteType(type->returnType);
      break;
    case TYPE_STRUCT:
      for (int i = 0; i < bufLength(type->fieldTypes); i++) {
        deleteType(type->fieldTypes[i]);
      }
      bufFree(type->fieldTypes);
      break;
  }

  free(type);
}


void deleteNode(ASTNode* node) {
  if (node == NULL) {
    return;
  }

  switch (node->kind) {
    case NODE_NONE:
    case NODE_ERROR:
      break;

    case NODE_DECL:
      switch (node->declKind) {
        case DECL_NONE:
        case DECL_FIELD:
        case DECL_PARAM:
          break;
        case DECL_VAR:
        case DECL_CONST:
          deleteNode(node->initValue);
          break;
        case DECL_FUNC:
          for (int i = 0; i < bufLength(node->parameters); i++) {
            deleteNode(node->parameters[i]);
          }
          bufFree(node->parameters);
          deleteNode(node->funcBlock);
          break;
        case DECL_STRUCT:
          for (int i = 0; i < bufLength(node->fields); i++) {
            deleteNode(node->fields[i]);
          }
          bufFree(node->fields);
          break;
      }
      break;

    case NODE_STMT:
      switch (node->stmtKind) {
        case STMT_NONE:
        case STMT_BREAK:
        case STMT_CONTINUE:
          break;
        case STMT_BLOCK:
          for (int i = 0; i < bufLength(node->statements); i++) {
            deleteNode(node->statements[i]);
          }
          bufFree(node->statements);
          break;
        case STMT_EXPR:
        case STMT_RETURN:
          deleteNode(node->expression);
          break;
        case STMT_ASSIGN:
          deleteNode(node->lvalue);
          deleteNode(node->expression);
          break;
        case STMT_IF:
        case STMT_ELSEIF:
          deleteNode(node->condition);
          deleteNode(node->ifBlock);
          deleteNode(node->elseBlock);
          break;
        case STMT_ELSE:
          for (int i = 0; i < bufLength(node->statements); i++) {
            deleteNode(node->statements[i]);
          }
          bufFree(node->statements);
          break;
        case STMT_FOR:
          deleteNode(node->initDecl);
          deleteNode(node->postStmt);
        case STMT_WHILE:
        case STMT_DOWHILE:
          deleteNode(node->condition);
          deleteNode(node->loopBody);
          break;
        case STMT_SWITCH:
          deleteNode(node->expression);
          for (int i = 0; i < bufLength(node->cases); i++) {
            deleteNode(node->cases[i]);
          }
          bufFree(node->cases);
          break;
        case STMT_CASE:
          deleteNode(node->condition);
          deleteNode(node->caseBlock);
          break;
      }
      break;

    case NODE_EXPR:
      switch (node->exprKind) {
        case EXPR_NONE:
        case EXPR_INT:
        case EXPR_BOOL:
        case EXPR_NAME:
          break;
        case EXPR_BINARY:
          deleteNode(node->left);
          deleteNode(node->right);
          break;
        case EXPR_CALL:
          deleteNode(node->operand);
          for (int i = 0; i < bufLength(node->arguments); i++) {
            deleteNode(node->arguments[i]);
          }
          bufFree(node->arguments);
          break;
        case EXPR_INDEX:
          deleteNode(node->index);
        case EXPR_UNARY:
        case EXPR_FIELD:
          deleteNode(node->operand);
          break;
      }
      break;
  }

  free(node);
}
