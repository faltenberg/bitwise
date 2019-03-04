#ifndef __TOKEN_H__
#define __TOKEN_H__


typedef enum TokenKind {
  TOKEN_OP_PLUS,
  TOKEN_OP_MINUS,
  TOKEN_OP_ADD,
  TOKEN_OP_INC,
  TOKEN_OP_SUB,
  TOKEN_OP_DEC,
  TOKEN_OP_MUL,
  TOKEN_OP_DIV,
  TOKEN_OP_MOD,
} TokenKind;


#endif  // __TOKEN_H__
