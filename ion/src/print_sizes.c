#include "arena.h"
#include "ast.h"
#include "error.h"
#include "lexer.h"
#include "loc.h"
#include "number.h"
#include "parser.h"
#include "sbuffer.h"
#include "source.h"
#include "str.h"
#include "token.h"

#include <stdbool.h>
#include <stdio.h>


#define PROMPT "%3luB  %s\n"
#define PRINT_SIZE(type) printf(PROMPT, sizeof(type), #type);


int main() {
  printf("<system>\n");
  PRINT_SIZE(bool);
  PRINT_SIZE(char);
  PRINT_SIZE(short);
  PRINT_SIZE(int);
  PRINT_SIZE(long);
  PRINT_SIZE(float);
  PRINT_SIZE(double);
  PRINT_SIZE(size_t);
  PRINT_SIZE(void*);
  printf("\n");

  printf("<arena.h>\n");
  PRINT_SIZE(Arena);
  printf("\n");

  printf("<sbuffer.h>\n");
  PRINT_SIZE(SBUF(int));
  printf("\n");

  printf("<str.h>\n");
  PRINT_SIZE(string);
  printf("\n");

  printf("<number.h>\n");
  PRINT_SIZE(Number);
  printf("\n");

  printf("<source.h>\n");
  PRINT_SIZE(SourceStatus);
  PRINT_SIZE(Source);
  printf("\n");

  printf("<loc.h>\n");
  PRINT_SIZE(Location);
  printf("\n");

  printf("<error.h>\n");
  PRINT_SIZE(Error);
  printf("\n");

  printf("<token.h>\n");
  PRINT_SIZE(TokenKind);
  PRINT_SIZE(Token);
  printf("\n");

  printf("<lexer.h>\n");
  PRINT_SIZE(Lexer);
  printf("\n");

  printf("<ast.h>\n");
  PRINT_SIZE(ExprKind);
  PRINT_SIZE(ASTExpr);
  PRINT_SIZE(ASTKind);
  PRINT_SIZE(ASTNode);
  printf("\n");
}
