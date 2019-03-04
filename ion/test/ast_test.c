#include "cunit.h"

#include "ast.h"


static TestResult testCreatePrimitiveExpressions() {
  TestResult result = {};

  {
    ASTNode* node = createExprInt(42);
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_EXPR));
    ABORT(assertEqualInt(node->exprKind, EXPR_INT));
    TEST(assertEqualInt(node->intValue, 42));
    deleteNode(node);
  }

  {
    ASTNode* node = createExprBool(true);
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_EXPR));
    ABORT(assertEqualInt(node->exprKind, EXPR_BOOL));
    TEST(assertTrue(node->boolValue));
    deleteNode(node);
  }

  {
    ASTNode* node = createExprName("foo");
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_EXPR));
    ABORT(assertEqualInt(node->exprKind, EXPR_NAME));
    TEST(assertEqualString(node->nameLiteral, "foo"));
    deleteNode(node);
  }

  return result;
}


static TestResult testCreateOperatorExpressions() {
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
    ABORT(assertEqualInt(node->exprKind, EXPR_BINARY));
    TEST(assertEqualInt(node->binop, TOKEN_OP_ADD));
    ABORT(assertNotNull(node->left));
    ABORT(assertNotNull(node->right));
    TEST(assertEqualString(node->left->nameLiteral, "x"));
    TEST(assertEqualInt(node->right->intValue, 1));
    deleteNode(node);
  }

  return result;
}


static TestResult testCreateFunctionCallExpressions() {
  TestResult result = {};

  {
    ASTNode* node = createExprCall(createExprName("f"), 0);
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_EXPR));
    ABORT(assertEqualInt(node->exprKind, EXPR_CALL));
    ABORT(assertNotNull(node->operand));
    TEST(assertEqualString(node->operand->nameLiteral, "f"));
    ABORT(assertEqualSize(bufLength(node->arguments), 0));
    deleteNode(node);
  }

  {
    ASTNode* node = createExprCall(createExprName("f"), 1, createExprInt(42));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_EXPR));
    ABORT(assertEqualInt(node->exprKind, EXPR_CALL));
    ABORT(assertEqualSize(bufLength(node->arguments), 1));
    ABORT(assertNotNull(node->arguments[0]));
    TEST(assertEqualInt(node->arguments[0]->intValue, 42));
    deleteNode(node);
  }

  {
    ASTNode* node = createExprCall(createExprName("f"), 3,
                                   createExprInt(10), createExprInt(11), createExprInt(12));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_EXPR));
    ABORT(assertEqualInt(node->exprKind, EXPR_CALL));
    ABORT(assertEqualSize(bufLength(node->arguments), 3));
    for (int i = 0; i < 3; i++) {
      ABORT(assertNotNull(node->arguments[i]));
      TEST(assertEqualInt(node->arguments[i]->intValue, 10+i));
    }
    deleteNode(node);
  }

  return result;
}


static TestResult testCreateArrayIndexingExpressions() {
  TestResult result = {};

  {
    ASTNode* node = createExprIndex(createExprName("a"), createExprInt(1));
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_EXPR));
    ABORT(assertEqualInt(node->exprKind, EXPR_INDEX));
    ABORT(assertNotNull(node->operand));
    TEST(assertEqualString(node->operand->nameLiteral, "a"));
    ABORT(assertNotNull(node->index));
    TEST(assertEqualInt(node->index->intValue, 1));
    deleteNode(node);
  }

  return result;
}


static TestResult testCreateFieldAccessExpressions() {
  TestResult result = {};

  {
    ASTNode* node = createExprField(createExprName("s"), "field");
    ABORT(assertNotNull(node));
    TEST(assertEqualInt(node->kind, NODE_EXPR));
    ABORT(assertEqualInt(node->exprKind, EXPR_FIELD));
    ABORT(assertNotNull(node->operand));
    TEST(assertEqualString(node->operand->nameLiteral, "s"));
    TEST(assertEqualString(node->fieldName, "field"));
    deleteNode(node);
  }

  return result;
}


static TestResult testCreateTypes() {
  TestResult result = {};

  {
    Type* type = createTypeNone();
    ABORT(assertNotNull(type));
    TEST(assertEqualInt(type->kind, TYPE_NONE));
    deleteType(type);
  }

  {
    Type* type = createTypeName("Vector");
    ABORT(assertNotNull(type));
    ABORT(assertEqualInt(type->kind, TYPE_NAME));
    TEST(assertEqualString(type->name, "Vector"));
    deleteType(type);
  }

  {
    Type* type = createTypePointer(createTypeName("int"));
    ABORT(assertNotNull(type));
    ABORT(assertEqualInt(type->kind, TYPE_POINTER));
    ABORT(assertNotNull(type->baseType));
    ABORT(assertEqualInt(type->baseType->kind, TYPE_NAME));
    TEST(assertEqualString(type->baseType->name, "int"));
    deleteType(type);
  }

  {
    Type* type = createTypeArray(createTypeName("int"), 10);
    ABORT(assertNotNull(type));
    ABORT(assertEqualInt(type->kind, TYPE_ARRAY));
    ABORT(assertNotNull(type->baseType));
    ABORT(assertEqualInt(type->baseType->kind, TYPE_NAME));
    TEST(assertEqualString(type->baseType->name, "int"));
    TEST(assertEqualInt(type->arraySize, 10));
    deleteType(type);
  }

  {
    Type* type = createTypeFunc(createTypeName("int"), 0);
    ABORT(assertNotNull(type));
    ABORT(assertEqualInt(type->kind, TYPE_FUNC));
    ABORT(assertNotNull(type->returnType));
    ABORT(assertEqualInt(type->returnType->kind, TYPE_NAME));
    TEST(assertEqualString(type->returnType->name, "int"));
    TEST(assertEqualInt(bufLength(type->paramTypes), 0));
    deleteType(type);
  }

  {
    Type* type = createTypeFunc(createTypeNone(), 1, createTypeName("int"));
    ABORT(assertNotNull(type));
    ABORT(assertEqualInt(type->kind, TYPE_FUNC));
    ABORT(assertNotNull(type->returnType));
    TEST(assertEqualInt(type->returnType->kind, TYPE_NONE));
    ABORT(assertEqualInt(bufLength(type->paramTypes), 1));
    ABORT(assertNotNull(type->paramTypes[0]));
    ABORT(assertEqualInt(type->paramTypes[0]->kind, TYPE_NAME));
    TEST(assertEqualString(type->paramTypes[0]->name, "int"));
    deleteType(type);
  }

  {
    Type* type = createTypeFunc(createTypeNone(), 2, createTypeName("int"), createTypeName("bool"));
    ABORT(assertNotNull(type));
    ABORT(assertEqualInt(type->kind, TYPE_FUNC));
    ABORT(assertNotNull(type->returnType));
    TEST(assertEqualInt(type->returnType->kind, TYPE_NONE));
    ABORT(assertEqualInt(bufLength(type->paramTypes), 2));
    ABORT(assertNotNull(type->paramTypes[0]));
    ABORT(assertEqualInt(type->paramTypes[0]->kind, TYPE_NAME));
    TEST(assertEqualString(type->paramTypes[0]->name, "int"));
    ABORT(assertNotNull(type->paramTypes[1]));
    ABORT(assertEqualInt(type->paramTypes[1]->kind, TYPE_NAME));
    TEST(assertEqualString(type->paramTypes[1]->name, "bool"));
    deleteType(type);
  }

  {
    Type* type = createTypeStruct("Vector", 0);
    ABORT(assertNotNull(type));
    ABORT(assertEqualInt(type->kind, TYPE_STRUCT));
    TEST(assertEqualString(type->name, "Vector"));
    TEST(assertEqualInt(bufLength(type->fieldTypes), 0));
    deleteType(type);
  }

  {
    Type* type = createTypeStruct("Vector", 1, createTypeName("int"));
    ABORT(assertNotNull(type));
    ABORT(assertEqualInt(type->kind, TYPE_STRUCT));
    TEST(assertEqualString(type->name, "Vector"));
    TEST(assertEqualInt(bufLength(type->fieldTypes), 1));
    ABORT(assertNotNull(type->fieldTypes[0]));
    ABORT(assertEqualInt(type->fieldTypes[0]->kind, TYPE_NAME));
    TEST(assertEqualString(type->fieldTypes[0]->name, "int"));
    deleteType(type);
  }

  {
    Type* type = createTypeStruct("Vector", 2, createTypeName("int"), createTypeName("bool"));
    ABORT(assertNotNull(type));
    ABORT(assertEqualInt(type->kind, TYPE_STRUCT));
    TEST(assertEqualString(type->name, "Vector"));
    TEST(assertEqualInt(bufLength(type->fieldTypes), 2));
    ABORT(assertNotNull(type->fieldTypes[0]));
    ABORT(assertEqualInt(type->fieldTypes[0]->kind, TYPE_NAME));
    TEST(assertEqualString(type->fieldTypes[0]->name, "int"));
    ABORT(assertNotNull(type->fieldTypes[1]));
    ABORT(assertEqualInt(type->fieldTypes[1]->kind, TYPE_NAME));
    TEST(assertEqualString(type->fieldTypes[1]->name, "bool"));
    deleteType(type);
  }

  return result;
}


TestResult ast_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<ast>", "Test abstract syntax trees.");
  addTest(&suite, &testCreatePrimitiveExpressions,     "testCreatePrimitiveExpressions");
  addTest(&suite, &testCreateOperatorExpressions,      "testCreateOperatorExpressions");
  addTest(&suite, &testCreateFunctionCallExpressions,  "testCreateFunctionCallExpreesions");
  addTest(&suite, &testCreateArrayIndexingExpressions, "testCreateArrayIndexingExpressions");
  addTest(&suite, &testCreateFieldAccessExpressions,   "testCreateFieldAccessExpressions");
  addTest(&suite, &testCreateTypes,                    "testCreateTypes");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
