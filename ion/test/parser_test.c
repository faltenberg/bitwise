#include "cunit.h"
#include "util.h"

#include "parser.h"

#include "error.h"
#include "strintern.h"


GENERATE_ASSERT_EQUAL_ENUM(ASTKind)
GENERATE_ASSERT_EQUAL_ENUM(ExprKind)


static Number num(const char* n) {
  return numFromString(stringFromArray(n));
}


#define assertASTNode(n, k) __assertASTNode(__FILE__, __LINE__, n, k)
static bool __assertASTNode(const char* file, int line, const ASTNode* node, ASTKind kind) {
  if (!__assertNotNull(file, line, node)) {
    return false;
  }
  if (!__assertEqualEnum(ASTKind, file, line, node->kind, kind)) {
    return false;
  }
  return true;
}


#define assertASTExpr(n, k) __assertASTExpr(__FILE__, __LINE__, n, k)
static bool __assertASTExpr(const char* file, int line, const ASTNode* node, ExprKind kind) {
  if (!__assertASTNode(file, line, node, AST_EXPR)) {
    return false;
  }
  if (!__assertEqualEnum(ExprKind, file, line, node->expr.kind, kind)) {
    return false;
  }
  return true;
}


/********************************************* TESTS *********************************************/


static TestResult testParseEmptyString() {
  TestResult result = {};

  {
    Source src = sourceFromString("");
    ASTNode* node = parse(&src);
    TEST(assertASTNode(node, AST_NONE));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString(" ");
    ASTNode* node = parse(&src);
    TEST(assertASTNode(node, AST_NONE));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("\n");
    ASTNode* node = parse(&src);
    TEST(assertASTNode(node, AST_NONE));
    deleteNode(node);
    deleteSource(&src);
  }

  return result;
}


static TestResult testParseErrors() {
  TestResult result = {};

  {
    Source src = sourceFromString("$");
    ASTNode* node = parse(&src);
    TEST(assertASTNode(node, AST_ERROR));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("if");
    ASTNode* node = parse(&src);
    TEST(assertASTNode(node, AST_ERROR));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("_");
    ASTNode* node = parse(&src);
    TEST(assertASTNode(node, AST_ERROR));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("123x");
    ASTNode* node = parse(&src);
    TEST(assertASTNode(node, AST_ERROR));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("// comment");
    ASTNode* node = parse(&src);
    TEST(assertASTNode(node, AST_ERROR));
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
    ABORT(assertASTExpr(node, EXPR_NAME));
    TEST(assertEqualStr(node->expr.name, "x"));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("_1x2A");
    ASTNode* node = parse(&src);
    ABORT(assertASTExpr(node, EXPR_NAME));
    TEST(assertEqualStr(node->expr.name, "_1x2A"));
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
    ABORT(assertASTExpr(node, EXPR_INT));
    TEST(assertEqualNumber(node->expr.value, num("123")));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("0b__1111_0000");
    ASTNode* node = parse(&src);
    ABORT(assertASTExpr(node, EXPR_INT));
    TEST(assertEqualNumber(node->expr.value, num("0b__1111_0000")));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("0x_1234_ABCD");
    ASTNode* node = parse(&src);
    ABORT(assertASTExpr(node, EXPR_INT));
    TEST(assertEqualNumber(node->expr.value, num("0x_1234_ABCD")));
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
    ABORT(assertASTExpr(node, EXPR_UNOP));
    TEST(assertEqualStr(node->expr.op, "-"));
    TEST(assertASTExpr(node->expr.rhs, EXPR_NAME));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString(" +x");
    ASTNode* node = parse(&src);
    ABORT(assertASTExpr(node, EXPR_UNOP));
    TEST(assertEqualStr(node->expr.op, "+"));
    TEST(assertASTExpr(node->expr.rhs, EXPR_NAME));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("!valid");
    ASTNode* node = parse(&src);
    ABORT(assertASTExpr(node, EXPR_UNOP));
    TEST(assertEqualStr(node->expr.op, "!"));
    TEST(assertASTExpr(node->expr.rhs, EXPR_NAME));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("~mask");
    ASTNode* node = parse(&src);
    ABORT(assertASTExpr(node, EXPR_UNOP));
    TEST(assertEqualStr(node->expr.op, "~"));
    TEST(assertASTExpr(node->expr.rhs, EXPR_NAME));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("!!x");
    ASTNode* node = parse(&src);
    ABORT(assertASTExpr(node, EXPR_UNOP));
    TEST(assertEqualStr(node->expr.op, "!"));
    ABORT(assertASTExpr(node->expr.rhs, EXPR_UNOP));
    TEST(assertEqualStr(node->expr.rhs->expr.op, "!"));
    ABORT(assertASTExpr(node->expr.rhs->expr.rhs, EXPR_NAME));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("/x");
    ASTNode* node = parse(&src);
    string msg = generateError(&src, loc(1, 1), loc(1, 1), loc(1, 1),
                               "invalid unary operator /");
    ABORT(assertASTNode(node, AST_ERROR));
    ABORT(assertEqualInt(sbufLength(node->messages), 1));
    TEST(assertEqualStr(node->messages[0], msg.chars));
    TEST(assertASTNode(node->faultyNode, AST_NONE));
    strFree(&msg);
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("+");
    ASTNode* node = parse(&src);
    string msg = generateError(&src, loc(1, 2), loc(1, 2), loc(1, 2),
                               "missing operand");
    ABORT(assertASTNode(node, AST_ERROR));
    ABORT(assertEqualInt(sbufLength(node->messages), 2));
    TEST(assertEqualStr(node->messages[0], msg.chars));
    TEST(assertASTExpr(node->faultyNode, EXPR_UNOP));
    strFree(&msg);
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("!if");
    ASTNode* node = parse(&src);
    string msg = generateError(&src, loc(1, 2), loc(1, 2), loc(1, 3),
                               "missing operand");
    ABORT(assertASTNode(node, AST_ERROR));
    ABORT(assertEqualInt(sbufLength(node->messages), 2));
    TEST(assertEqualStr(node->messages[0], msg.chars));
    TEST(assertASTExpr(node->faultyNode, EXPR_UNOP));
    strFree(&msg);
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("!#x");
    ASTNode* node = parse(&src);
    string msg = generateError(&src, loc(1, 2), loc(1, 2), loc(1, 2),
                               "missing operand");
    ABORT(assertASTNode(node, AST_ERROR));
    ABORT(assertEqualInt(sbufLength(node->messages), 2));
    TEST(assertEqualStr(node->messages[0], msg.chars));
    TEST(assertASTExpr(node->faultyNode, EXPR_UNOP));
    strFree(&msg);
    deleteNode(node);
    deleteSource(&src);
  }

  return result;
}

/*
static TestResult testParseExprParen() {
  TestResult result = {};

  {
    Source src = sourceFromString("(x)");
    ASTNode* node = parse(&src);
    ABORT(assertASTExpr(node, EXPR_PAREN));
    TEST(assertASTExpr(node->expr.expr, EXPR_NAME));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("  (x");
    ASTNode* node = parse(&src);
    string msg = generateError(&src, loc(1, 3), loc(1, 5), loc(1, 5), "missing closing ')'");
    string note = generateNote(&src, loc(1, 3), loc(1, 3), loc(1, 3), "to match this '('");
    ABORT(assertASTNode(node, AST_ERROR));
    ABORT(assertEqualInt(sbufLength(node->messages), 2));
    TEST(assertEqualStr(node->messages[0], msg.chars));
    TEST(assertEqualStr(node->messages[1], note.chars));
    TEST(assertASTExpr(node->faultyNode, EXPR_PAREN));
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
    ABORT(assertASTNode(node, AST_ERROR));
    ABORT(assertEqualInt(sbufLength(node->messages), 2));
    TEST(assertEqualStr(node->messages[0], msg.chars));
    TEST(assertEqualStr(node->messages[1], note.chars));
    TEST(assertASTExpr(node->faultyNode, EXPR_PAREN));
    strFree(&msg);
    strFree(&note);
    deleteNode(node);
    deleteSource(&src);
  }

  return result;
}
*/

static TestResult testParseExprArithmeticBinop() {
  TestResult result = {};

  {
    Source src = sourceFromString("x + y");
    ASTNode* node = parse(&src);
    ABORT(assertASTExpr(node, EXPR_BINOP));
    TEST(assertEqualStr(node->expr.op, "+"));
    TEST(assertASTExpr(node->expr.lhs, EXPR_NAME));
    TEST(assertASTExpr(node->expr.rhs, EXPR_NAME));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("x - y");
    ASTNode* node = parse(&src);
    ABORT(assertASTExpr(node, EXPR_BINOP));
    TEST(assertEqualStr(node->expr.op, "-"));
    TEST(assertASTExpr(node->expr.lhs, EXPR_NAME));
    TEST(assertASTExpr(node->expr.rhs, EXPR_NAME));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("x * y");
    ASTNode* node = parse(&src);
    ABORT(assertASTExpr(node, EXPR_BINOP));
    TEST(assertEqualStr(node->expr.op, "*"));
    TEST(assertASTExpr(node->expr.lhs, EXPR_NAME));
    TEST(assertASTExpr(node->expr.rhs, EXPR_NAME));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("x / y");
    ASTNode* node = parse(&src);
    ABORT(assertASTExpr(node, EXPR_BINOP));
    TEST(assertEqualStr(node->expr.op, "/"));
    TEST(assertASTExpr(node->expr.lhs, EXPR_NAME));
    TEST(assertASTExpr(node->expr.rhs, EXPR_NAME));
    deleteNode(node);
    deleteSource(&src);
  }

  {
    Source src = sourceFromString("x % y");
    ASTNode* node = parse(&src);
    ABORT(assertASTExpr(node, EXPR_BINOP));
    TEST(assertEqualStr(node->expr.op, "%"));
    TEST(assertASTExpr(node->expr.lhs, EXPR_NAME));
    TEST(assertASTExpr(node->expr.rhs, EXPR_NAME));
    deleteNode(node);
    deleteSource(&src);
  }

  return result;
}


static TestResult testParseExprBinopAssociativity() {
  TestResult result = {};

  {
    Source src = sourceFromString("x + y + z");
    ASTNode* node = parse(&src);
    ABORT(assertASTExpr(node, EXPR_BINOP));
    TEST(assertEqualStr(node->expr.op, "+"));
    TEST(assertASTExpr(node->expr.lhs, EXPR_BINOP));
    TEST(assertASTExpr(node->expr.rhs, EXPR_NAME));
    deleteNode(node);
    deleteSource(&src);
  }

  return result;
}


TestResult parser_alltests(PrintLevel verbosity) {
  TestSuite suite = newSuite("TestSuite<parser>", "Test parser.");
  addTest(&suite, testParseEmptyString);
  addTest(&suite, testParseErrors);
  addTest(&suite, testParseExprName);
  addTest(&suite, testParseExprInt);
  addTest(&suite, testParseExprUnary);
//  addTest(&suite, testParseExprParen);
  addTest(&suite, testParseExprArithmeticBinop);
  addTest(&suite, testParseExprBinopAssociativity);
  TestResult result = run(&suite, verbosity);
  deleteSuite(&suite);
  strinternFree();
  return result;
}

