#include "parser.h"

#include "token.h"
#include "lexer.h"
#include "error.h"

#include <stdlib.h>
#include <stdio.h>



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


static string UNOP_PLUS;
static string UNOP_MINUS;
static string UNOP_NOT;
static string UNOP_NEG;
static string LPAREN;
static string RPAREN;


static void init() {
  static bool initialized = false;
  if (!initialized) {
    UNOP_PLUS = stringFromArray("+");
    UNOP_MINUS = stringFromArray("-");
    UNOP_NOT = stringFromArray("!");
    UNOP_NEG = stringFromArray("~");
    LPAREN = stringFromArray("(");
    RPAREN = stringFromArray(")");
    initialized = true;
  }
}


static ASTNode* createNode(ASTKind kind) {
  ASTNode* node = (ASTNode*) calloc(1, sizeof(ASTNode));
  node->kind = kind;
  return node;
}


static ASTNode* createErrorNode() {
  ASTNode* node = createNode(AST_ERROR);
  return node;
}


static ASTNode* createExprNode(ExprKind kind) {
  ASTNode* node = createNode(AST_EXPR);
  node->expr.kind = kind;
  return node;
}


static ASTNode* parseExpr(Parser* lexer);  // forward declaration


static ASTNode* parseExprInt(Parser* parser) {
  ASTNode* node = createExprNode(EXPR_INT);
  node->expr.value = numFromString(parser->currentToken.chars);
  return node;
}


static ASTNode* parseExprName(Parser* parser) {
  ASTNode* node = createExprNode(EXPR_NAME);
  node->expr.name = parser->currentToken.chars;
  return node;
}


static ASTNode* parseExprParen(Parser* parser) {
  Token lparen = parser->currentToken;
  ASTNode* node = createExprNode(EXPR_PAREN);
  node->expr.expr = parseExpr(parser);
  Token rparen = peek(parser);
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


static ASTNode* parseExprUnop(Parser* parser) {
  ASTNode* node = createExprNode(EXPR_UNOP);
  node->expr.op = parser->currentToken.chars;
  node->expr.rhs = parseExpr(parser);
  return node;
}


static ASTNode* parseExpr(Parser* parser) {
  Token token = next(parser);

  switch (token.kind) {
    case TOKEN_INT:
      return parseExprInt(parser);

    case TOKEN_NAME:
      return parseExprName(parser);

    case TOKEN_SYMBOL:
      if (strequal(token.chars, UNOP_MINUS) ||
          strequal(token.chars, UNOP_PLUS) ||
          strequal(token.chars, UNOP_NOT) ||
          strequal(token.chars, UNOP_NEG)
         ) {
        return parseExprUnop(parser);
      } else if (strequal(token.chars, LPAREN)) {
        return parseExprParen(parser);
      }

    default:
      return createNode(AST_NONE);
  }
}


ASTNode* parse(const Source* src) {
  init();
  Lexer lexer = lexerFromSource(src);
  Parser parser = createParser(&lexer);
  return parseExpr(&parser);
}
