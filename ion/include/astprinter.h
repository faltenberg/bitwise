#ifndef __ASTPRINTER_H__
#define __ASTPRINTER_H__

#include "ast.h"

/**
 * AST Printer
 * ===========
 *
 * AST printer provides a print function that traverses a syntax tree and prints each node in a
 * LISP-like fashion for visual introspection. There is also a print function for `Type`s.
 *
 * Example
 * ```````
 *
 * ```c
 * #include "ast.h"
 * #include "astprinter.h"
 *
 * int main() {
 *   ASTNode* node = createEmptyNode();  // create some AST
 *   printlnAST(node);
 * }
 * ```
 */


void printAST(const ASTNode* node);

void printlnAST(const ASTNode* node);

void printType(const Type* type);

void printlnType(const Type* type);


#endif  // __ASTPRINTER_H__
