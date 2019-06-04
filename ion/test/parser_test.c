#include "cunit.h"
#include "util.h"

#include "parser.h"

#include "error.h"


GENERATE_ASSERT_EQUAL_ENUM(ASTKind)
GENERATE_ASSERT_EQUAL_ENUM(ExprKind)


static Number num(const char* n) {
  return numFromString(stringFromArray(n));
}


static TestResult testParseEmptyString() {
  TestResult result = {};

  {
    Source src = sourceFromString("");
    ASTNode* node = parse(&src);
    ABORT(assertNotNull(node));
    TEST(assertEqualEnum(ASTKind, node->kind, AST_NONE));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString(" ");
    ASTNode* node = parse(&src);
    ABORT(assertNotNull(node));
    TEST(assertEqualEnum(ASTKind, node->kind, AST_NONE));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("\n");
    ASTNode* node = parse(&src);
    ABORT(assertNotNull(node));
    TEST(assertEqualEnum(ASTKind, node->kind, AST_NONE));
    deleteNode(node);
    deleteSource(&src);
  }

  return result;
}


static TestResult testParseExprInt() {
  TestResult result = {};

  {
    Source src = sourceFromString("123");
    ASTNode* node = parse(&src);
    ABORT(assertNotNull(node));
    TEST(assertEqualEnum(ASTKind, node->kind, AST_EXPR));
    ABORT(assertEqualEnum(ExprKind, node->expr.kind, EXPR_INT));
    TEST(assertEqualNumber(node->expr.value, num("123")));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("0b__1111_0000");
    ASTNode* node = parse(&src);
    ABORT(assertNotNull(node));
    TEST(assertEqualEnum(ASTKind, node->kind, AST_EXPR));
    ABORT(assertEqualEnum(ExprKind, node->expr.kind, EXPR_INT));
    TEST(assertEqualNumber(node->expr.value, num("0b__1111_0000")));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("0x_1234_ABCD");
    ASTNode* node = parse(&src);
    ABORT(assertNotNull(node));
    TEST(assertEqualEnum(ASTKind, node->kind, AST_EXPR));
    ABORT(assertEqualEnum(ExprKind, node->expr.kind, EXPR_INT));
    TEST(assertEqualNumber(node->expr.value, num("0x_1234_ABCD")));
    deleteNode(node);
    deleteSource(&src);
  }

  return result;
}


static TestResult testParseExprName() {
  TestResult result = {};

  {
    Source src = sourceFromString("x");
    ASTNode* node = parse(&src);
    ABORT(assertNotNull(node));
    TEST(assertEqualEnum(ASTKind, node->kind, AST_EXPR));
    ABORT(assertEqualEnum(ExprKind, node->expr.kind, EXPR_NAME));
    TEST(assertEqualStr(node->expr.name, "x"));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("_");
    ASTNode* node = parse(&src);
    ABORT(assertNotNull(node));
    TEST(assertNotEqualEnum(ASTKind, node->kind, AST_EXPR));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("_1x2A");
    ASTNode* node = parse(&src);
    ABORT(assertNotNull(node));
    TEST(assertEqualEnum(ASTKind, node->kind, AST_EXPR));
    ABORT(assertEqualEnum(ExprKind, node->expr.kind, EXPR_NAME));
    TEST(assertEqualStr(node->expr.name, "_1x2A"));
    deleteNode(node);
    deleteSource(&src);
  }

  return result;
}


static TestResult testParseExprUnary() {
  TestResult result = {};

  {
    Source src = sourceFromString("-x");
    ASTNode* node = parse(&src);
    ABORT(assertNotNull(node));
    TEST(assertEqualEnum(ASTKind, node->kind, AST_EXPR));
    ABORT(assertEqualEnum(ExprKind, node->expr.kind, EXPR_UNOP));
    ABORT(assertNotNull(node->expr.rhs));
    TEST(assertEqualEnum(ASTKind, node->expr.rhs->kind, AST_EXPR));
    TEST(assertEqualEnum(ExprKind, node->expr.rhs->expr.kind, EXPR_NAME));
    TEST(assertEqualStr(node->expr.op, "-"));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString(" +x");
    ASTNode* node = parse(&src);
    ABORT(assertNotNull(node));
    TEST(assertEqualEnum(ASTKind, node->kind, AST_EXPR));
    ABORT(assertEqualEnum(ExprKind, node->expr.kind, EXPR_UNOP));
    ABORT(assertNotNull(node->expr.rhs));
    TEST(assertEqualEnum(ASTKind, node->expr.rhs->kind, AST_EXPR));
    TEST(assertEqualEnum(ExprKind, node->expr.rhs->expr.kind, EXPR_NAME));
    TEST(assertEqualStr(node->expr.op, "+"));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("!valid");
    ASTNode* node = parse(&src);
    ABORT(assertNotNull(node));
    TEST(assertEqualEnum(ASTKind, node->kind, AST_EXPR));
    ABORT(assertEqualEnum(ExprKind, node->expr.kind, EXPR_UNOP));
    ABORT(assertNotNull(node->expr.rhs));
    TEST(assertEqualEnum(ASTKind, node->expr.rhs->kind, AST_EXPR));
    TEST(assertEqualEnum(ExprKind, node->expr.rhs->expr.kind, EXPR_NAME));
    TEST(assertEqualStr(node->expr.op, "!"));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("~mask");
    ASTNode* node = parse(&src);
    ABORT(assertNotNull(node));
    TEST(assertEqualEnum(ASTKind, node->kind, AST_EXPR));
    ABORT(assertEqualEnum(ExprKind, node->expr.kind, EXPR_UNOP));
    ABORT(assertNotNull(node->expr.rhs));
    TEST(assertEqualEnum(ASTKind, node->expr.rhs->kind, AST_EXPR));
    TEST(assertEqualEnum(ExprKind, node->expr.rhs->expr.kind, EXPR_NAME));
    TEST(assertEqualStr(node->expr.op, "~"));
    deleteNode(node);
    deleteSource(&src);
  }

  return result;
}


static TestResult testParseExprArithmeticBinop() {
  TestResult result = {};

  {
    Source src = sourceFromString("x + y");
    ASTNode* node = parse(&src);
    ABORT(assertNotNull(node));
    TEST(assertEqualEnum(ASTKind, node->kind, AST_EXPR));
    ABORT(assertEqualEnum(ExprKind, node->expr.kind, EXPR_BINOP));
    ABORT(assertNotNull(node->expr.lhs));
    ABORT(assertNotNull(node->expr.rhs));
    TEST(assertEqualEnum(ASTKind, node->expr.lhs->kind, AST_EXPR));
    TEST(assertEqualEnum(ExprKind, node->expr.lhs->expr.kind, EXPR_NAME));
    TEST(assertEqualEnum(ASTKind, node->expr.rhs->kind, AST_EXPR));
    TEST(assertEqualEnum(ExprKind, node->expr.rhs->expr.kind, EXPR_NAME));
    TEST(assertEqualStr(node->expr.op, "+"));
    deleteNode(node);
    deleteSource(&src);
  }

  return result;
}


static TestResult testParseExprParen() {
  TestResult result = {};

  {
    Source src = sourceFromString("(x)");
    ASTNode* node = parse(&src);
    ABORT(assertNotNull(node));
    TEST(assertEqualEnum(ASTKind, node->kind, AST_EXPR));
    ABORT(assertEqualEnum(ExprKind, node->expr.kind, EXPR_PAREN));
    ABORT(assertNotNull(node->expr.expr));
    TEST(assertEqualEnum(ASTKind, node->expr.expr->kind, AST_EXPR));
    TEST(assertEqualEnum(ExprKind, node->expr.expr->expr.kind, EXPR_NAME));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("  (x");
    ASTNode* node = parse(&src);
    string msg = generateError(&src, loc(1, 3), loc(1, 5), loc(1, 5), "missing closing ')'");
    string note = generateNote(&src, loc(1, 3), loc(1, 3), loc(1, 3), "to match this '('");
    ABORT(assertNotNull(node));
    TEST(assertEqualEnum(ASTKind, node->kind, AST_ERROR));
    ABORT(assertEqualInt(sbufLength(node->messages), 2));
    TEST(assertEqualStr(node->messages[0], msg.chars));
    TEST(assertEqualStr(node->messages[1], note.chars));
    ABORT(assertNotNull(node->faultyNode));
    TEST(assertEqualEnum(ASTKind, node->faultyNode->kind, AST_EXPR));
    TEST(assertEqualEnum(ExprKind, node->faultyNode->expr.kind, EXPR_PAREN));
    strFree(&msg);
    strFree(&note);
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("((x)");
    ASTNode* node = parse(&src);
    string msg = generateError(&src, loc(1, 1), loc(1, 5), loc(1, 5), "missing closing ')'");
    string note = generateNote(&src, loc(1, 1), loc(1, 1), loc(1, 1), "to match this '('");
    ABORT(assertNotNull(node));
    TEST(assertEqualEnum(ASTKind, node->kind, AST_ERROR));
    ABORT(assertEqualInt(sbufLength(node->messages), 2));
    TEST(assertEqualStr(node->messages[0], msg.chars));
    TEST(assertEqualStr(node->messages[1], note.chars));
    ABORT(assertNotNull(node->faultyNode));
    TEST(assertEqualEnum(ASTKind, node->faultyNode->kind, AST_EXPR));
    TEST(assertEqualEnum(ExprKind, node->faultyNode->expr.kind, EXPR_PAREN));
    strFree(&msg);
    strFree(&note);
    deleteNode(node);
    deleteSource(&src);
  }

  return result;
}


TestResult parser_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<parser>", "Test parser.");
  addTest(&suite, &testParseEmptyString,         "testParseEmptyString");
  addTest(&suite, &testParseExprInt,             "testParseExprInt");
  addTest(&suite, &testParseExprName,            "testParseExprName");
  addTest(&suite, &testParseExprUnary,           "testParseExprUnary");
  addTest(&suite, &testParseExprArithmeticBinop, "testParseExprArithmeticBinop");
  addTest(&suite, &testParseExprParen,           "testParseExprParen");
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  return result;
}
