#ifndef __ASTPRINTER_H__
#define __ASTPRINTER_H__


/**
 * AST Printer
 * ===========
 *
 * AST printer provides a print function that traverses a syntax tree and prints each node in a
 * LISP-like fashion for visual introspection. There is also a print function for `Type`s.
 *
 *
 * Example
 * ```````
 *
 * ```c {.line-numbers}
 * #include "ast.h"
 * #include "astprinter.h"
 *
 * int main() {
 *   Type* type = createTypeNone();  // create some type
 *   ASTNode* node = createEmptyNode();  // create some AST
 *
 *   printlnType(type);
 *   printlnAST(node);
 *
 *   deleteNode(node);
 *   deleteType(type);
 * }
 * ```
 */


#include "ast.h"


/**
 * Prints the whole syntax tree.
 *
 * - **param:** `node` - the root of the syntax tree
 */
void printAST(const ASTNode* node);


/**
 * Prints the whole syntax tree. Prints an ending `'\n'` at the end, so one doesn't have to.
 *
 * - **param:** `node` - the root of the syntax tree
 */
void printlnAST(const ASTNode* node);


/**
 * Prints the type information of a type.
 *
 * - **param:** `type` - the type tor print
 */
void printType(const Type* type);


/**
 * Prints the type information of a type. Prints an ending `'\n'` at the end, so one doesn't
 *  have to.
 *
 * - **param:** `type` - the type tor print
 */
void printlnType(const Type* type);


#endif  // __ASTPRINTER_H__
