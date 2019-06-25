#include "parser.h"

#include "token.h"
#include "lexer.h"
#include "error.h"
#include "strintern.h"

#include <stdlib.h>
#include <stdio.h>


/********************************************* PARSER ********************************************/


typedef struct Parser {
  Lexer* lexer;
  Token  currentToken;
  Token  nextToken;
} Parser;


static Parser createParser(Lexer* lexer) {
  return (Parser){ .lexer=lexer, .currentToken=(Token){ .kind=TOKEN_NONE },
                   .nextToken=(Token){ .kind=TOKEN_NONE }
                 };
}


static Token next(Parser* parser) {
  if (parser->nextToken.kind == TOKEN_NONE) {
    parser->currentToken = nextToken(parser->lexer);
  } else {
    parser->currentToken = parser->nextToken;
    parser->nextToken = (Token){ .kind=TOKEN_NONE };
  }
  return parser->currentToken;
}


static Token peek(Parser* parser) {
  if (parser->nextToken.kind == TOKEN_NONE) {
    parser->nextToken = nextToken(parser->lexer);
  }
  return parser->nextToken;
}


static string LPAREN;
static string RPAREN;
static string UNOP_PLUS;
static string UNOP_MINUS;
static string UNOP_NOT;
static string UNOP_NEG;
static string BINOP_ADD;
static string BINOP_SUB;
static string BINOP_MUL;
static string BINOP_DIV;
static string BINOP_MOD;


static bool initialized = false;
static void init() {
  if (!initialized) {
    strintern("() !~+-*/%");
    LPAREN = strintern("(");
    RPAREN = strintern(")");
    UNOP_PLUS = strintern("+");
    UNOP_MINUS = strintern("-");
    UNOP_NOT = strintern("!");
    UNOP_NEG = strintern("~");
    BINOP_ADD = strintern("+");
    BINOP_SUB = strintern("-");
    BINOP_MUL = strintern("*");
    BINOP_DIV = strintern("/");
    BINOP_MOD = strintern("%");
    initialized = true;
  }
}


/****************************************** CREATE NODES *****************************************/


static ASTNode* createNode(ASTKind kind) {
  ASTNode* node = (ASTNode*) calloc(1, sizeof(ASTNode));
  node->kind = kind;
  return node;
}


static ASTNode* createEmptyNode() {
  return createNode(AST_NONE);
}


static ASTNode* createErrorNode() {
  return createNode(AST_ERROR);
}


static ASTNode* createTokenNoneError(const Parser* parser) {
  Token token = parser->currentToken;
  ASTNode* node = createErrorNode();
  string msg = generateError(token.source, token.start, token.start, token.end,
                             "Token[TOKEN_NONE] should not appear - how did it happen?");
  sbufPush(node->messages, msg);
  node->faultyNode = createEmptyNode();
  return node;
}


static ASTNode* createUnexpectedTokenError(const Parser* parser) {
  Token token = parser->currentToken;
  ASTNode* node = createErrorNode();
  if (token.kind == TOKEN_ERROR) {
    sbufPush(node->messages, token.error->message);
  } else {
    string msg = generateError(token.source, token.start, token.start, token.end,
                               (token.chars.len > 0) ? "unexpected Token[%s %.*s]"
                                                     : "unexpected Token[%s]",
                               strTokenKind(token.kind), token.chars.len, token.chars.chars);
    sbufPush(node->messages, msg);
  }
  node->faultyNode = createEmptyNode();
  return node;
}


static ASTNode* createExprNode(ExprKind kind) {
  ASTNode* node = createNode(AST_EXPR);
  node->expr.kind = kind;
  return node;
}


/******************************************** PARSING ********************************************/


static ASTNode* parseExpr(Parser* lexer);  // forward declaration
static ASTNode* parseTerm(Parser* lexer);  // forward declaration


/*

static ASTNode* parseExprParen(Parser* parser) {
  ASTNode* node = createExprNode(EXPR_PAREN);
  Token lparen = parser->currentToken;
  Token rparen = peek(parser);
  if (rparen.kind == TOKEN_SYMBOL && strequal(rparen.chars, RPAREN)) {
    next(parser);
    ASTNode* error = createErrorNode();
    sbufPush(error->messages,
             generateError(parser->lexer->source, lparen.start, rparen.start, rparen.end,
                           "missing expression"));
    node->expr.expr = createEmptyNode();
    error->faultyNode = node;
    return error;
  }

  node->expr.expr = parseExpr(parser);
  rparen = peek(parser);
  if (rparen.kind == TOKEN_SYMBOL && strequal(rparen.chars, RPAREN)) {
    next(parser);
    return node;
  } else {
    ASTNode* error = createErrorNode();
    sbufPush(error->messages,
             generateError(parser->lexer->source, lparen.start, rparen.end, rparen.end,
                           "missing closing ')'"));
    sbufPush(error->messages,
             generateNote(parser->lexer->source, lparen.start, lparen.start, lparen.end,
                          "to match this '('"));
    error->faultyNode = node;
    return error;
  }
}
*/


static ASTNode* parseExprName(Parser* parser) {
  ASTNode* node = createExprNode(EXPR_NAME);
  string name = parser->currentToken.chars;
  node->expr.name = strinternRange(name.chars, name.chars + name.len);
  return node;
}


static ASTNode* parseExprInt(Parser* parser) {
  ASTNode* node = createExprNode(EXPR_INT);
  node->expr.value = numFromString(parser->currentToken.chars);
  return node;
}


static ASTNode* parseExprUnop(Parser* parser) {
  Token token = parser->currentToken;
  ASTNode* rhs = parseTerm(parser);
  ASTNode* node = createExprNode(EXPR_UNOP);
  node->expr.op = strinternRange(token.chars.chars, token.chars.chars + token.chars.len);
  node->expr.rhs = rhs;
  if (rhs->kind == AST_EXPR) {
    return node;
  } else {
    ASTNode* error = createErrorNode();
    Token current = parser->currentToken;
    string msg = generateError(current.source, current.start, current.start, current.end,
                             "missing operand");
    sbufPush(error->messages, msg);
    string note = generateNote(token.source, token.start, token.start, token.end,
                               "for unary operator %.*s", token.chars.len, token.chars.chars);
    sbufPush(error->messages, note);
    error->faultyNode = node;
    return error;
  }
}


static ASTNode* parseExprBinop(Parser* parser, ASTNode* lhs) {
  Token token = next(parser);
  ASTNode* rhs = parseExpr(parser);
  ASTNode* node = createExprNode(EXPR_BINOP);
  node->expr.op = strinternRange(token.chars.chars, token.chars.chars + token.chars.len);
  node->expr.lhs = lhs;
  node->expr.rhs = rhs;
  if (rhs->kind == AST_EXPR || true) {
    // restore associativity
    if (rhs->expr.kind == EXPR_BINOP) {
      node->expr.rhs = rhs->expr.lhs;
      rhs->expr.lhs = node;
      node = rhs;
    }
    return node;
  } else {
    ASTNode* error = createErrorNode();
    Token current = parser->currentToken;
    string msg = generateError(current.source, current.start, current.start, current.end,
                             "missing operand");
    sbufPush(error->messages, msg);
    string note = generateNote(token.source, token.start, token.start, token.end,
                               "for binary operator %.*s", token.chars.len, token.chars.chars);
    sbufPush(error->messages, note);
    error->faultyNode = node;
    return error;
  }
}


static ASTNode* parseTerm(Parser* parser) {
  Token token = next(parser);
  switch (token.kind) {
    case TOKEN_NAME:
      return parseExprName(parser);

    case TOKEN_INT:
      return parseExprInt(parser);

    case TOKEN_SYMBOL:
      if (strequal(token.chars, UNOP_PLUS) ||
          strequal(token.chars, UNOP_MINUS) ||
          strequal(token.chars, UNOP_NOT) ||
          strequal(token.chars, UNOP_NEG)) {
        return parseExprUnop(parser);
      } else if (strequal(token.chars, LPAREN)) {
//        return parseExprParen(parser);
      } else {
        ASTNode* error = createErrorNode();
        string msg = generateError(token.source, token.start, token.start, token.end,
                                   "invalid unary operator %.*s",
                                   token.chars.len, token.chars.chars);
        sbufPush(error->messages, msg);
        error->faultyNode = createEmptyNode();
        return error;
      }

    default:
      return createUnexpectedTokenError(parser);
  }
}


static ASTNode* parseExpr(Parser* parser) {
  ASTNode* term = parseTerm(parser);

  Token token = peek(parser);
  switch (token.kind) {
    case TOKEN_SYMBOL:
      if (strequal(token.chars, BINOP_ADD) ||
          strequal(token.chars, BINOP_SUB) ||
          strequal(token.chars, BINOP_MUL) ||
          strequal(token.chars, BINOP_DIV) ||
          strequal(token.chars, BINOP_MOD)) {
        return parseExprBinop(parser, term);
      }

    default:
      return term;
  }
}


static ASTNode* parseStart(Parser* parser) {
  Token token = peek(parser);
  if (token.kind == TOKEN_EOF) {
    return createEmptyNode();
  }

  ASTNode* node = parseExpr(parser);

  token = next(parser);
  if (token.kind == TOKEN_EOF || node->kind == AST_ERROR) {
    return node;
  } else {
    ASTNode* error = createErrorNode();
    string msg = generateError(token.source, token.start, token.start, token.end,
                               "expected Token[TOKEN_EOF]");
    sbufPush(error->messages, msg);
    error->faultyNode = node;
    return error;
  }
}


ASTNode* parse(const Source* src) {
  init();
  Lexer lexer = lexerFromSource(src);
  Parser parser = createParser(&lexer);
  ASTNode* node = parseStart(&parser);
  initialized = false;
  return node;
}
