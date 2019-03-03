#include "cunit.h"

#include "ast.h"


static TestResult testNodeDeletion() {
  TestResult result = {};

  {
    ASTNode* node = NULL;
    deleteNode(&node);
    TEST(assertNull(node));
  }

  {
    ASTNode* node = createEmptyNode();
    ABORT(assertNotNull(node));
    deleteNode(&node);
    TEST(assertNull(node));
  }

  {
    ASTNode* node = createEmptyNode();
    ABORT(assertNotNull(node));
    node->kind = NODE_STMT;
    node->stmtKind = STMT_EXPR;
    node->expression = createEmptyNode();
    ASTNode* *anker = &(node->expression);
    ABORT(assertNotNull(anker));
    ABORT(assertNotNull(*anker));
    deleteNode(&node);
    TEST(assertNull(node));
    TEST(assertNull(*anker));
  }

  return result;
}


static TestResult testCreatePrimitiveExpressions() {
  TestResult result = {};

  {
    ASTNode* node = createExprInt(42);
    ABORT(assertEqualInt(node->kind, NODE_EXPR));
    ABORT(assertEqualInt(node->exprKind, EXPR_INT));
    TEST(assertEqualInt(node->intValue, 42));
    deleteNode(&node);
  }

  {
    ASTNode* node = createExprBool(true);
    ABORT(assertEqualInt(node->kind, NODE_EXPR));
    ABORT(assertEqualInt(node->exprKind, EXPR_BOOL));
    TEST(assertTrue(node->boolValue));
    deleteNode(&node);
  }

  {
    ASTNode* node = createExprName("foo");
    ABORT(assertEqualInt(node->kind, NODE_EXPR));
    ABORT(assertEqualInt(node->exprKind, EXPR_NAME));
    TEST(assertEqualString(node->nameLiteral, "foo"));
    deleteNode(&node);
  }

  return result;
}


static TestResult testCreateOperatorExpressions() {
  TestResult result = {};

  {
    ASTNode* node = createExprUnary(TOKEN_OP_MINUS, createExprName("foo"));
    ABORT(assertEqualInt(node->kind, NODE_EXPR));
    ABORT(assertEqualInt(node->exprKind, EXPR_UNARY));
    TEST(assertEqualInt(node->unop, TOKEN_OP_MINUS));
    ABORT(assertNotNull(node->operand));
    TEST(assertEqualString(node->operand->nameLiteral, "foo"));
    deleteNode(&node);
  }

  {
    ASTNode* node = createExprBinary(createExprName("x"), TOKEN_OP_ADD, createExprInt(1));
    ABORT(assertEqualInt(node->kind, NODE_EXPR));
    ABORT(assertEqualInt(node->exprKind, EXPR_BINARY));
    TEST(assertEqualInt(node->binop, TOKEN_OP_ADD));
    ABORT(assertNotNull(node->left));
    ABORT(assertNotNull(node->right));
    TEST(assertEqualString(node->left->nameLiteral, "x"));
    TEST(assertEqualInt(node->right->intValue, 1));
    deleteNode(&node);
  }

  return result;
}


static TestResult testCreateFunctionCallExpressions() {
  TestResult result = {};

  {
    ASTNode* node = createExprCall(createExprName("f"), 0);
    ABORT(assertEqualInt(node->kind, NODE_EXPR));
    ABORT(assertEqualInt(node->exprKind, EXPR_CALL));
    ABORT(assertNotNull(node->operand));
    TEST(assertEqualString(node->operand->nameLiteral, "f"));
    ABORT(assertEqualSize(bufLength(node->arguments), 0));
    deleteNode(&node);
  }

  {
    ASTNode* node = createExprCall(createExprName("f"), 1, createExprInt(42));
    ABORT(assertEqualSize(bufLength(node->arguments), 1));
    ABORT(assertNotNull(node->arguments[0]));
    TEST(assertEqualInt(node->arguments[0]->intValue, 42));
    deleteNode(&node);
  }

  {
    ASTNode* node = createExprCall(createExprName("f"), 3,
                                   createExprInt(10), createExprInt(11), createExprInt(12));
    ABORT(assertEqualSize(bufLength(node->arguments), 3));
    for (int i = 0; i < 3; i++) {
      ABORT(assertNotNull(node->arguments[i]));
      TEST(assertEqualInt(node->arguments[i]->intValue, 10+i));
    }
    deleteNode(&node);
  }

  return result;
}


static TestResult testCreateArrayIndexingExpressions() {
  TestResult result = {};

  {
    ASTNode* node = createExprIndex(createExprName("a"), createExprInt(1));
    ABORT(assertEqualInt(node->kind, NODE_EXPR));
    ABORT(assertEqualInt(node->exprKind, EXPR_INDEX));
    ABORT(assertNotNull(node->operand));
    TEST(assertEqualString(node->operand->nameLiteral, "a"));
    ABORT(assertNotNull(node->index));
    TEST(assertEqualInt(node->index->intValue, 1));
    deleteNode(&node);
  }

  return result;
}


static TestResult testCreateFieldAccessExpressions() {
  TestResult result = {};

  {
    ASTNode* node = createExprField(createExprName("s"), "field");
    ABORT(assertEqualInt(node->kind, NODE_EXPR));
    ABORT(assertEqualInt(node->exprKind, EXPR_FIELD));
    ABORT(assertNotNull(node->operand));
    TEST(assertEqualString(node->operand->nameLiteral, "s"));
    TEST(assertEqualString(node->fieldName, "field"));
    deleteNode(&node);
  }

  return result;
}


TestResult ast_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<ast>", "Test abstract syntax trees.");
  addTest(&suite, &testNodeDeletion,                   "testNodeDeletion");
  addTest(&suite, &testCreatePrimitiveExpressions,     "testCreatePrimitiveExpressions");
  addTest(&suite, &testCreateOperatorExpressions,      "testCreateOperatorExpressions");
  addTest(&suite, &testCreateFunctionCallExpressions,  "testCreateFunctionCallExpreesions");
  addTest(&suite, &testCreateArrayIndexingExpressions, "testCreateArrayIndexingExpressions");
  addTest(&suite, &testCreateFieldAccessExpressions,   "testCreateFieldAccessExpressions");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
