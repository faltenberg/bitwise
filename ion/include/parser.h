#ifndef __PARSER_H__
#define __PARSER_H__


/**
 * Parser
 * ======
 *
 * ...
 *
 *
 * Example
 * -------
 *
 * ```c {.line-numbers}
 * #include "parser.h"
 *
 * int main() {
 *   // ...
 * }
 * ```
 */


#include "source.h"
#include "ast.h"


ASTNode* parse(const Source* src);


#endif  // __PARSER_H__
