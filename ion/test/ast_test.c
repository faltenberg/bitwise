#include "cunit.h"

#include "ast.h"


static TestResult testCreateTypeNone() {
  TestResult result = {};
  Type* type = createTypeNone();
  ABORT(assertNotNull(type));
  TEST(assertEqualInt(type->kind, TYPE_NONE));
  deleteType(type);
  return result;
}


static TestResult testCreateTypeName() {
  TestResult result = {};
  Type* type = createTypeName("Vector");
  ABORT(assertNotNull(type));
  TEST(assertEqualInt(type->kind, TYPE_NAME));
  TEST(assertEqualString(type->name, "Vector"));
  deleteType(type);
  return result;
}


static TestResult testCreateTypePointer() {
  TestResult result = {};
  Type* type = createTypePointer(createTypeName("int"));
  ABORT(assertNotNull(type));
  TEST(assertEqualInt(type->kind, TYPE_POINTER));
  ABORT(assertNotNull(type->baseType));
  TEST(assertEqualInt(type->baseType->kind, TYPE_NAME));
  TEST(assertEqualString(type->baseType->name, "int"));
  deleteType(type);
  return result;
}


static TestResult testCreateTypeArray() {
  TestResult result = {};
  Type* type = createTypeArray(createTypeName("int"), 10);
  ABORT(assertNotNull(type));
  TEST(assertEqualInt(type->kind, TYPE_ARRAY));
  ABORT(assertNotNull(type->baseType));
  TEST(assertEqualInt(type->baseType->kind, TYPE_NAME));
  TEST(assertEqualString(type->baseType->name, "int"));
  TEST(assertEqualInt(type->arraySize, 10));
  deleteType(type);
  return result;
}


static TestResult testCreateTypeFunc() {
  TestResult result = {};

  {
    Type* type = createTypeFunc(createTypeName("int"), 0);
    ABORT(assertNotNull(type));
    TEST(assertEqualInt(type->kind, TYPE_FUNC));
    ABORT(assertNotNull(type->returnType));
    TEST(assertEqualInt(type->returnType->kind, TYPE_NAME));
    TEST(assertEqualString(type->returnType->name, "int"));
    TEST(assertEqualInt(bufLength(type->paramTypes), 0));
    deleteType(type);
  }

  {
    Type* type = createTypeFunc(createTypeNone(), 1, createTypeName("int"));
    ABORT(assertNotNull(type));
    TEST(assertEqualInt(type->kind, TYPE_FUNC));
    ABORT(assertNotNull(type->returnType));
    TEST(assertEqualInt(type->returnType->kind, TYPE_NONE));
    ABORT(assertEqualInt(bufLength(type->paramTypes), 1));
    ABORT(assertNotNull(type->paramTypes[0]));
    TEST(assertEqualInt(type->paramTypes[0]->kind, TYPE_NAME));
    TEST(assertEqualString(type->paramTypes[0]->name, "int"));
    deleteType(type);
  }

  {
    Type* type = createTypeFunc(createTypeNone(), 2, createTypeName("int"), createTypeName("bool"));
    ABORT(assertNotNull(type));
    TEST(assertEqualInt(type->kind, TYPE_FUNC));
    ABORT(assertNotNull(type->returnType));
    TEST(assertEqualInt(type->returnType->kind, TYPE_NONE));
    ABORT(assertEqualInt(bufLength(type->paramTypes), 2));
    ABORT(assertNotNull(type->paramTypes[0]));
    TEST(assertEqualInt(type->paramTypes[0]->kind, TYPE_NAME));
    TEST(assertEqualString(type->paramTypes[0]->name, "int"));
    ABORT(assertNotNull(type->paramTypes[1]));
    TEST(assertEqualInt(type->paramTypes[1]->kind, TYPE_NAME));
    TEST(assertEqualString(type->paramTypes[1]->name, "bool"));
    deleteType(type);
  }

  return result;
}


static TestResult testCreateTypeStruct() {
  TestResult result = {};

  {
    Type* type = createTypeStruct("Vector", 0);
    ABORT(assertNotNull(type));
    TEST(assertEqualInt(type->kind, TYPE_STRUCT));
    TEST(assertEqualString(type->name, "Vector"));
    TEST(assertEqualInt(bufLength(type->fieldTypes), 0));
    deleteType(type);
  }

  {
    Type* type = createTypeStruct("Vector", 1, createTypeName("int"));
    ABORT(assertNotNull(type));
    TEST(assertEqualInt(type->kind, TYPE_STRUCT));
    TEST(assertEqualString(type->name, "Vector"));
    TEST(assertEqualInt(bufLength(type->fieldTypes), 1));
    ABORT(assertNotNull(type->fieldTypes[0]));
    TEST(assertEqualInt(type->fieldTypes[0]->kind, TYPE_NAME));
    TEST(assertEqualString(type->fieldTypes[0]->name, "int"));
    deleteType(type);
  }

  {
    Type* type = createTypeStruct("Vector", 2, createTypeName("int"), createTypeName("bool"));
    ABORT(assertNotNull(type));
    TEST(assertEqualInt(type->kind, TYPE_STRUCT));
    TEST(assertEqualString(type->name, "Vector"));
    TEST(assertEqualInt(bufLength(type->fieldTypes), 2));
    ABORT(assertNotNull(type->fieldTypes[0]));
    TEST(assertEqualInt(type->fieldTypes[0]->kind, TYPE_NAME));
    TEST(assertEqualString(type->fieldTypes[0]->name, "int"));
    ABORT(assertNotNull(type->fieldTypes[1]));
    TEST(assertEqualInt(type->fieldTypes[1]->kind, TYPE_NAME));
    TEST(assertEqualString(type->fieldTypes[1]->name, "bool"));
    deleteType(type);
  }

  return result;
}


static TestResult testCreateEmptyNode() {
  TestResult result = {};
  ASTNode* node = createEmptyNode();
  ABORT(assertNotNull(node));
  TEST(assertEqualInt(node->kind, NODE_NONE));
  deleteNode(node);
  return result;
}


static TestResult testCreateErrorNode() {
  TestResult result = {};
  ASTNode* node = createErrorNode("error message");
  ABORT(assertNotNull(node));
  TEST(assertEqualInt(node->kind, NODE_ERROR));
  TEST(assertEqualString(node->errorMessage, "error message"));
  deleteNode(node);
  return result;
}


static TestResult testCreateExprPrimitive() {
  TestResult result = {};

  {
    ASTNode* node = createExprInt(42);
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_EXPR));
    TEST(assertEqualInt(node->exprKind, EXPR_INT));
    TEST(assertEqualInt(node->intValue, 42));
    deleteNode(node);
  }

  {
    ASTNode* node = createExprBool(true);
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_EXPR));
    TEST(assertEqualInt(node->exprKind, EXPR_BOOL));
    TEST(assertTrue(node->boolValue));
    deleteNode(node);
  }

  {
    ASTNode* node = createExprName("foo");
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_EXPR));
    TEST(assertEqualInt(node->exprKind, EXPR_NAME));
    TEST(assertEqualString(node->nameLiteral, "foo"));
    deleteNode(node);
  }

  return result;
}


static TestResult testCreateExprOperator() {
  TestResult result = {};

  {
    ASTNode* node = createExprUnary(TOKEN_OP_MINUS, createExprName("foo"));
    ABORT(assertEqualInt(node->exprKind, EXPR_UNARY));
    TEST(assertEqualInt(node->unop, TOKEN_OP_MINUS));
    ABORT(assertNotNull(node->operand));
    TEST(assertEqualString(node->operand->nameLiteral, "foo"));
    deleteNode(node);
  }

  {
    ASTNode* node = createExprBinary(createExprName("x"), TOKEN_OP_ADD, createExprInt(1));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_EXPR));
    TEST(assertEqualInt(node->exprKind, EXPR_BINARY));
    TEST(assertEqualInt(node->binop, TOKEN_OP_ADD));
    ABORT(assertNotNull(node->left));
    ABORT(assertNotNull(node->right));
    TEST(assertEqualString(node->left->nameLiteral, "x"));
    TEST(assertEqualInt(node->right->intValue, 1));
    deleteNode(node);
  }

  return result;
}


static TestResult testCreateExprCall() {
  TestResult result = {};

  {
    ASTNode* node = createExprCall(createExprName("f"), 0);
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_EXPR));
    TEST(assertEqualInt(node->exprKind, EXPR_CALL));
    ABORT(assertNotNull(node->operand));
    TEST(assertEqualString(node->operand->nameLiteral, "f"));
    TEST(assertEqualSize(bufLength(node->arguments), 0));
    deleteNode(node);
  }

  {
    ASTNode* node = createExprCall(createExprName("f"), 1, createExprInt(42));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_EXPR));
    TEST(assertEqualInt(node->exprKind, EXPR_CALL));
    ABORT(assertEqualSize(bufLength(node->arguments), 1));
    TEST(assertNotNull(node->arguments[0]));
    TEST(assertEqualInt(node->arguments[0]->intValue, 42));
    deleteNode(node);
  }

  {
    ASTNode* node = createExprCall(createExprName("f"), 3,
                                   createExprInt(10), createExprInt(11), createExprInt(12));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_EXPR));
    ABORT(assertEqualInt(node->exprKind, EXPR_CALL));
    TEST(assertEqualSize(bufLength(node->arguments), 3));
    for (int i = 0; i < 3; i++) {
      ABORT(assertNotNull(node->arguments[i]));
      TEST(assertEqualInt(node->arguments[i]->intValue, 10+i));
    }
    deleteNode(node);
  }

  return result;
}


static TestResult testCreateExprIndex() {
  TestResult result = {};

  {
    ASTNode* node = createExprIndex(createExprName("a"), createExprInt(1));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_EXPR));
    TEST(assertEqualInt(node->exprKind, EXPR_INDEX));
    ABORT(assertNotNull(node->operand));
    TEST(assertEqualString(node->operand->nameLiteral, "a"));
    ABORT(assertNotNull(node->index));
    TEST(assertEqualInt(node->index->intValue, 1));
    deleteNode(node);
  }

  return result;
}


static TestResult testCreateExprField() {
  TestResult result = {};

  {
    ASTNode* node = createExprField(createExprName("s"), "field");
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_EXPR));
    TEST(assertEqualInt(node->exprKind, EXPR_FIELD));
    ABORT(assertNotNull(node->operand));
    TEST(assertEqualString(node->operand->nameLiteral, "s"));
    TEST(assertEqualString(node->fieldName, "field"));
    deleteNode(node);
  }

  return result;
}


static TestResult testCreateStmtBlock() {
  TestResult result = {};

  {
    ASTNode* node = createStmtBlock(0);
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_STMT));
    TEST(assertEqualInt(node->stmtKind, STMT_BLOCK));
    TEST(assertEqualInt(bufLength(node->statements), 0));
    deleteNode(node);
  }

  {
    ASTNode* node = createStmtBlock(1, createStmtEmpty());
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_STMT));
    TEST(assertEqualInt(node->stmtKind, STMT_BLOCK));
    ABORT(assertEqualInt(bufLength(node->statements), 1));
    ABORT(assertNotNull(node->statements[0]));
    TEST(assertEqualInt(node->statements[0]->kind, NODE_STMT));
    TEST(assertEqualInt(node->statements[0]->stmtKind, STMT_NONE));
    deleteNode(node);
  }

  {
    ASTNode* node = createStmtBlock(2, createStmtBreak(), createStmtContinue());
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_STMT));
    TEST(assertEqualInt(node->stmtKind, STMT_BLOCK));
    ABORT(assertEqualInt(bufLength(node->statements), 2));
    ABORT(assertNotNull(node->statements[0]));
    TEST(assertEqualInt(node->statements[0]->kind, NODE_STMT));
    TEST(assertEqualInt(node->statements[0]->stmtKind, STMT_BREAK));
    ABORT(assertNotNull(node->statements[1]));
    TEST(assertEqualInt(node->statements[1]->kind, NODE_STMT));
    TEST(assertEqualInt(node->statements[1]->stmtKind, STMT_CONTINUE));
    deleteNode(node);
  }

  return result;
}


static TestResult testCreateStmtExpr() {
  TestResult result = {};
  ASTNode* node = createStmtExpr(createExprInt(2));
  ABORT(assertNotNull(node));
  TEST(assertEqualInt(node->kind, NODE_STMT));
  TEST(assertEqualInt(node->stmtKind, STMT_EXPR));
  ABORT(assertNotNull(node->expression));
  TEST(assertEqualInt(node->expression->kind, NODE_EXPR));
  deleteNode(node);
  return result;
}


static TestResult testCreateStmtReturn() {
  TestResult result = {};

  {
    ASTNode* node = createStmtReturn(createExprInt(2));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_STMT));
    TEST(assertEqualInt(node->stmtKind, STMT_RETURN));
    ABORT(assertNotNull(node->expression));
    TEST(assertEqualInt(node->expression->kind, NODE_EXPR));
    deleteNode(node);
  }

  {
    ASTNode* node = createStmtReturn(createEmptyNode());
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_STMT));
    TEST(assertEqualInt(node->stmtKind, STMT_RETURN));
    ABORT(assertNotNull(node->expression));
    TEST(assertEqualInt(node->expression->kind, NODE_NONE));
    deleteNode(node);
  }

  return result;
}


static TestResult testCreateStmtAssign() {
  TestResult result = {};

  {
    ASTNode* node = createStmtAssign(createExprName("x"), createExprInt(2));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_STMT));
    TEST(assertEqualInt(node->stmtKind, STMT_ASSIGN));
    ABORT(assertNotNull(node->lvalue));
    TEST(assertEqualInt(node->lvalue->kind, NODE_EXPR));
    TEST(assertEqualInt(node->opkind, TOKEN_ASSIGN));
    ABORT(assertNotNull(node->expression));
    TEST(assertEqualInt(node->expression->kind, NODE_EXPR));
    deleteNode(node);
  }

  {
    ASTNode* node = createStmtAssignOp(createExprName("x"), TOKEN_ASSIGN_ADD, createExprInt(2));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_STMT));
    TEST(assertEqualInt(node->stmtKind, STMT_ASSIGN));
    ABORT(assertNotNull(node->lvalue));
    TEST(assertEqualInt(node->lvalue->kind, NODE_EXPR));
    TEST(assertEqualInt(node->opkind, TOKEN_ASSIGN_ADD));
    ABORT(assertNotNull(node->expression));
    TEST(assertEqualInt(node->expression->kind, NODE_EXPR));
    deleteNode(node);
  }

  return result;
}


static TestResult testCreateStmtIf() {
  TestResult result = {};

  {
    ASTNode* node = createStmtIf(createExprBool(true), createStmtBlock(0), createStmtElse(0));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_STMT));
    TEST(assertEqualInt(node->stmtKind, STMT_IF));
    ABORT(assertNotNull(node->condition));
    TEST(assertEqualInt(node->condition->kind, NODE_EXPR));
    ABORT(assertNotNull(node->ifBlock));
    TEST(assertEqualInt(node->ifBlock->kind, NODE_STMT));
    TEST(assertEqualInt(node->ifBlock->stmtKind, STMT_BLOCK));
    ABORT(assertNotNull(node->elseBlock));
    TEST(assertEqualInt(node->elseBlock->kind, NODE_STMT));
    ABORT(assertEqualInt(node->elseBlock->stmtKind, STMT_ELSE));
    TEST(assertEqualInt(bufLength(node->elseBlock->statements), 0));
    deleteNode(node);
  }

  {
    ASTNode* node = createStmtIf(createExprBool(true), createStmtBlock(0),
                          createStmtElseIf(createExprBool(false), createStmtBlock(0),
                          createStmtElse(0)));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_STMT));
    TEST(assertEqualInt(node->stmtKind, STMT_IF));
    ABORT(assertNotNull(node->condition));
    TEST(assertEqualInt(node->condition->kind, NODE_EXPR));
    ABORT(assertNotNull(node->ifBlock));
    TEST(assertEqualInt(node->ifBlock->kind, NODE_STMT));
    TEST(assertEqualInt(node->ifBlock->stmtKind, STMT_BLOCK));
    ABORT(assertNotNull(node->elseBlock));
    TEST(assertEqualInt(node->elseBlock->kind, NODE_STMT));
    TEST(assertEqualInt(node->elseBlock->stmtKind, STMT_ELSEIF));
    ABORT(assertNotNull(node->elseBlock->condition));
    TEST(assertEqualInt(node->elseBlock->condition->kind, NODE_EXPR));
    ABORT(assertNotNull(node->elseBlock->ifBlock));
    TEST(assertEqualInt(node->elseBlock->ifBlock->kind, NODE_STMT));
    TEST(assertEqualInt(node->elseBlock->ifBlock->stmtKind, STMT_BLOCK));
    ABORT(assertNotNull(node->elseBlock->elseBlock));
    TEST(assertEqualInt(node->elseBlock->elseBlock->kind, NODE_STMT));
    TEST(assertEqualInt(node->elseBlock->elseBlock->stmtKind, STMT_ELSE));
    deleteNode(node);
  }

  return result;
}


static TestResult testCreateStmtWhile() {
  TestResult result = {};
  ASTNode* node = createStmtWhile(createExprBool(true), createStmtBlock(0));
  ABORT(assertNotNull(node));
  TEST(assertEqualInt(node->kind, NODE_STMT));
  TEST(assertEqualInt(node->stmtKind, STMT_WHILE));
  ABORT(assertNotNull(node->condition));
  TEST(assertEqualInt(node->condition->kind, NODE_EXPR));
  ABORT(assertNotNull(node->loopBody));
  TEST(assertEqualInt(node->loopBody->kind, NODE_STMT));
  TEST(assertEqualInt(node->loopBody->stmtKind, STMT_BLOCK));
  deleteNode(node);
  return result;
}


static TestResult testCreateStmtDoWhile() {
  TestResult result = {};
  ASTNode* node = createStmtDoWhile(createExprBool(true), createStmtBlock(0));
  ABORT(assertNotNull(node));
  TEST(assertEqualInt(node->kind, NODE_STMT));
  TEST(assertEqualInt(node->stmtKind, STMT_DOWHILE));
  ABORT(assertNotNull(node->condition));
  TEST(assertEqualInt(node->condition->kind, NODE_EXPR));
  ABORT(assertNotNull(node->loopBody));
  TEST(assertEqualInt(node->loopBody->kind, NODE_STMT));
  TEST(assertEqualInt(node->loopBody->stmtKind, STMT_BLOCK));
  deleteNode(node);
  return result;
}


static TestResult testCreateStmtFor() {
  TestResult result = {};
  ASTNode* node = createStmtFor(createDeclVar("i", createTypeName("int"), 0), createExprBool(true),
                        createStmtExpr(createExprUnary(TOKEN_OP_INC, createExprName("i"))),
                        createStmtBlock(0));
  ABORT(assertNotNull(node));
  TEST(assertEqualInt(node->kind, NODE_STMT));
  TEST(assertEqualInt(node->stmtKind, STMT_FOR));
  ABORT(assertNotNull(node->initDecl));
  TEST(assertEqualInt(node->initDecl->kind, NODE_DECL));
  ABORT(assertNotNull(node->condition));
  TEST(assertEqualInt(node->condition->kind, NODE_EXPR));
  ABORT(assertNotNull(node->postStmt));
  TEST(assertEqualInt(node->postStmt->kind, NODE_STMT));
  TEST(assertEqualInt(node->postStmt->stmtKind, STMT_EXPR));
  ABORT(assertNotNull(node->loopBody));
  TEST(assertEqualInt(node->loopBody->kind, NODE_STMT));
  TEST(assertEqualInt(node->loopBody->stmtKind, STMT_BLOCK));
  deleteType(node->initDecl->type);
  deleteNode(node);
  return result;
}


static TestResult testCreateStmtBreak() {
  TestResult result = {};
  ASTNode* node = createStmtBreak();
  ABORT(assertNotNull(node));
  TEST(assertEqualInt(node->kind, NODE_STMT));
  TEST(assertEqualInt(node->stmtKind, STMT_BREAK));
  deleteNode(node);
  return result;
}


static TestResult testCreateStmtContinue() {
  TestResult result = {};
  ASTNode* node = createStmtContinue();
  ABORT(assertNotNull(node));
  TEST(assertEqualInt(node->kind, NODE_STMT));
  TEST(assertEqualInt(node->stmtKind, STMT_CONTINUE));
  deleteNode(node);
  return result;
}


static TestResult testCreateStmtSwitch() {
  TestResult result = {};

  {
    ASTNode* node = createStmtSwitch(createExprName("t"), 0);
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_STMT));
    TEST(assertEqualInt(node->stmtKind, STMT_SWITCH));
    ABORT(assertNotNull(node->expression));
    TEST(assertEqualInt(node->expression->kind, NODE_EXPR));
    TEST(assertEqualInt(bufLength(node->cases), 0));
    deleteNode(node);
  }

  {
    ASTNode* node = createStmtSwitch(createExprName("t"), 1,
                          createStmtCase(createExprName("FOO"), createStmtBlock(0)));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_STMT));
    TEST(assertEqualInt(node->stmtKind, STMT_SWITCH));
    ABORT(assertNotNull(node->expression));
    TEST(assertEqualInt(node->expression->kind, NODE_EXPR));
    ABORT(assertEqualInt(bufLength(node->cases), 1));
    ABORT(assertNotNull(node->cases[0]));
    TEST(assertEqualInt(node->cases[0]->kind, NODE_STMT));
    ABORT(assertEqualInt(node->cases[0]->stmtKind, STMT_CASE));
    ABORT(assertNotNull(node->cases[0]->condition));
    TEST(assertEqualInt(node->cases[0]->condition->kind, NODE_EXPR));
    ABORT(assertNotNull(node->cases[0]->caseBlock));
    TEST(assertEqualInt(node->cases[0]->caseBlock->kind, NODE_STMT));
    TEST(assertEqualInt(node->cases[0]->caseBlock->stmtKind, STMT_BLOCK));
    deleteNode(node);
  }

  {
    ASTNode* node = createStmtSwitch(createExprName("t"), 2,
                          createStmtCase(createExprName("INT"), createStmtBlock(0)),
                          createStmtElse(0));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_STMT));
    TEST(assertEqualInt(node->stmtKind, STMT_SWITCH));
    ABORT(assertNotNull(node->expression));
    TEST(assertEqualInt(node->expression->kind, NODE_EXPR));
    ABORT(assertEqualInt(bufLength(node->cases), 2));
    ABORT(assertNotNull(node->cases[0]));
    TEST(assertEqualInt(node->cases[0]->kind, NODE_STMT));
    ABORT(assertEqualInt(node->cases[0]->stmtKind, STMT_CASE));
    ABORT(assertNotNull(node->cases[0]->condition));
    TEST(assertEqualInt(node->cases[0]->condition->kind, NODE_EXPR));
    ABORT(assertNotNull(node->cases[0]->caseBlock));
    TEST(assertEqualInt(node->cases[0]->caseBlock->kind, NODE_STMT));
    TEST(assertEqualInt(node->cases[0]->caseBlock->stmtKind, STMT_BLOCK));
    ABORT(assertNotNull(node->cases[1]));
    TEST(assertEqualInt(node->cases[1]->kind, NODE_STMT));
    ABORT(assertEqualInt(node->cases[1]->stmtKind, STMT_ELSE));
    TEST(assertEqualInt(bufLength(node->cases[1]->statements), 0));
    deleteNode(node);
  }

  return result;
}


static TestResult testCreateDeclVar() {
  TestResult result = {};
  ASTNode* node = createDeclVar("x", createTypeName("int"), createExprInt(2));
  ABORT(assertNotNull(node));
  TEST(assertEqualInt(node->kind, NODE_DECL));
  ABORT(assertEqualInt(node->declKind, DECL_VAR));
  TEST(assertEqualString(node->name, "x"));
  TEST(assertNotNull(node->type));
  TEST(assertNotNull(node->initValue));
  TEST(assertEqualInt(node->initValue->kind, NODE_EXPR));
  deleteType(node->type);
  deleteNode(node);
  return result;
}


static TestResult testCreateDeclConst() {
  TestResult result = {};
  ASTNode* node = createDeclConst("x", createTypeName("int"), createExprInt(2));
  ABORT(assertNotNull(node));
  TEST(assertEqualInt(node->kind, NODE_DECL));
  ABORT(assertEqualInt(node->declKind, DECL_CONST));
  TEST(assertEqualString(node->name, "x"));
  TEST(assertNotNull(node->type));
  TEST(assertNotNull(node->initValue));
  TEST(assertEqualInt(node->initValue->kind, NODE_EXPR));
  deleteType(node->type);
  deleteNode(node);
  return result;
}


static TestResult testCreateDeclStruct() {
  TestResult result = {};

  {
    ASTNode* node = createDeclStruct("Vector", 0);
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_DECL));
    ABORT(assertEqualInt(node->declKind, DECL_STRUCT));
    TEST(assertEqualString(node->name, "Vector"));
    ABORT(assertEqualInt(bufLength(node->fields), 0));
    deleteNode(node);
  }

  {
    ASTNode* node = createDeclStruct("Vector", 1, createDeclField("x", createTypeName("int")));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_DECL));
    ABORT(assertEqualInt(node->declKind, DECL_STRUCT));
    TEST(assertEqualString(node->name, "Vector"));
    ABORT(assertEqualInt(bufLength(node->fields), 1));
    ABORT(assertNotNull(node->fields[0]));
    TEST(assertEqualInt(node->fields[0]->kind, NODE_DECL));
    ABORT(assertEqualInt(node->fields[0]->declKind, DECL_FIELD));
    TEST(assertEqualString(node->fields[0]->name, "x"));
    TEST(assertNotNull(node->fields[0]->type));
    deleteType(node->fields[0]->type);
    deleteNode(node);
  }

  {
    ASTNode* node = createDeclStruct("Vector", 2,
                          createDeclField("x", createTypeName("int")),
                          createDeclField("y", createTypeName("int")));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_DECL));
    ABORT(assertEqualInt(node->declKind, DECL_STRUCT));
    TEST(assertEqualString(node->name, "Vector"));
    ABORT(assertEqualInt(bufLength(node->fields), 2));
    ABORT(assertNotNull(node->fields[0]));
    TEST(assertEqualInt(node->fields[0]->kind, NODE_DECL));
    ABORT(assertEqualInt(node->fields[0]->declKind, DECL_FIELD));
    TEST(assertEqualString(node->fields[0]->name, "x"));
    TEST(assertNotNull(node->fields[0]->type));
    ABORT(assertNotNull(node->fields[1]));
    TEST(assertEqualInt(node->fields[1]->kind, NODE_DECL));
    ABORT(assertEqualInt(node->fields[1]->declKind, DECL_FIELD));
    TEST(assertEqualString(node->fields[1]->name, "y"));
    TEST(assertNotNull(node->fields[1]->type));
    deleteType(node->fields[0]->type);
    deleteType(node->fields[1]->type);
    deleteNode(node);
  }

  return result;
}


static TestResult testCreateDeclFunc() {
  TestResult result = {};

  {
    ASTNode* node = createDeclFunc("f", createStmtBlock(0), createTypeNone(), 0);
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_DECL));
    ABORT(assertEqualInt(node->declKind, DECL_FUNC));
    TEST(assertEqualString(node->name, "f"));
    ABORT(assertNotNull(node->funcBlock));
    TEST(assertEqualInt(node->funcBlock->kind, NODE_STMT));
    TEST(assertEqualInt(node->funcBlock->stmtKind, STMT_BLOCK));
    ABORT(assertNotNull(node->returnType));
    TEST(assertEqualInt(node->returnType->kind, TYPE_NONE));
    TEST(assertEqualInt(bufLength(node->parameters), 0));
    deleteType(node->returnType);
    deleteNode(node);
  }

  {
    ASTNode* node = createDeclFunc("f", createStmtBlock(0), createTypeName("int"), 1,
                          createDeclParam("a", createTypeName("int")));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_DECL));
    ABORT(assertEqualInt(node->declKind, DECL_FUNC));
    TEST(assertEqualString(node->name, "f"));
    ABORT(assertNotNull(node->returnType));
    TEST(assertEqualInt(node->returnType->kind, TYPE_NAME));
    ABORT(assertEqualInt(bufLength(node->parameters), 1));
    ABORT(assertNotNull(node->parameters[0]));
    TEST(assertEqualInt(node->parameters[0]->kind, NODE_DECL));
    ABORT(assertEqualInt(node->parameters[0]->declKind, DECL_PARAM));
    TEST(assertEqualString(node->parameters[0]->name, "a"));
    TEST(assertNotNull(node->parameters[0]->type));
    deleteType(node->returnType);
    deleteType(node->parameters[0]->type);
    deleteNode(node);
  }

  {
    ASTNode* node = createDeclFunc("f", createStmtBlock(0), createTypeName("int"), 2,
                          createDeclParam("a", createTypeName("int")),
                          createDeclParam("b", createTypeName("int")));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_DECL));
    ABORT(assertEqualInt(node->declKind, DECL_FUNC));
    TEST(assertEqualString(node->name, "f"));
    ABORT(assertEqualInt(bufLength(node->parameters), 2));
    ABORT(assertNotNull(node->parameters[0]));
    TEST(assertEqualInt(node->parameters[0]->kind, NODE_DECL));
    ABORT(assertEqualInt(node->parameters[0]->declKind, DECL_PARAM));
    TEST(assertEqualString(node->parameters[0]->name, "a"));
    TEST(assertNotNull(node->parameters[0]->type));
    ABORT(assertNotNull(node->parameters[1]));
    TEST(assertEqualInt(node->parameters[1]->kind, NODE_DECL));
    ABORT(assertEqualInt(node->parameters[1]->declKind, DECL_PARAM));
    TEST(assertEqualString(node->parameters[1]->name, "b"));
    TEST(assertNotNull(node->parameters[1]->type));
    deleteType(node->returnType);
    deleteType(node->parameters[0]->type);
    deleteType(node->parameters[1]->type);
    deleteNode(node);
  }

  return result;
}


TestResult ast_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<ast>", "Test abstract syntax trees.");
  addTest(&suite, &testCreateTypeNone,      "testCreateTypeNone");
  addTest(&suite, &testCreateTypeName,      "testCreateTypeName");
  addTest(&suite, &testCreateTypeArray,     "testCreateTypeArray");
  addTest(&suite, &testCreateTypePointer,   "testCreateTypePointer");
  addTest(&suite, &testCreateTypeFunc,      "testCreateTypeFunc");
  addTest(&suite, &testCreateTypeStruct,    "testCreateTypeStruct");
  addTest(&suite, &testCreateEmptyNode,     "testCreateEmptyNode");
  addTest(&suite, &testCreateErrorNode,     "testCreateErrorNode");
  addTest(&suite, &testCreateExprPrimitive, "testCreateExprPrimitive");
  addTest(&suite, &testCreateExprOperator,  "testCreateExprOperator");
  addTest(&suite, &testCreateExprCall,      "testCreateExprCall");
  addTest(&suite, &testCreateExprIndex,     "testCreateExprIndex");
  addTest(&suite, &testCreateExprField,     "testCreateExprField");
  addTest(&suite, &testCreateStmtBlock,     "testCreateStmtBlock");
  addTest(&suite, &testCreateStmtExpr,      "testCreateStmtExpr");
  addTest(&suite, &testCreateStmtReturn,    "testCreateStmtReturn");
  addTest(&suite, &testCreateStmtAssign,    "testCreateStmtAssign");
  addTest(&suite, &testCreateStmtIf,        "testCreateStmtIf");
  addTest(&suite, &testCreateStmtWhile,     "testCreateStmtWhile");
  addTest(&suite, &testCreateStmtDoWhile,   "testCreateStmtDoWhile");
  addTest(&suite, &testCreateStmtFor,       "testCreateStmtFor");
  addTest(&suite, &testCreateStmtBreak,     "testCreateStmtBreak");
  addTest(&suite, &testCreateStmtContinue,  "testCreateStmtContinue");
  addTest(&suite, &testCreateStmtSwitch,    "testCreateStmtSwitch");
  addTest(&suite, &testCreateDeclVar,       "testCreateDeclVar");
  addTest(&suite, &testCreateDeclConst,     "testCreateDeclConst");
  addTest(&suite, &testCreateDeclStruct,    "testCreateDeclStruct");
  addTest(&suite, &testCreateDeclFunc,      "testCreateDeclFunc");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
