#ifndef LANG_H_INCLUDED
#define LANG_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "lib.h"

enum BinOpType {
  T_PLUS,
  T_MINUS,
  T_MUL,
  T_DIV,
  T_MOD,
  T_LT,
  T_GT,
  T_LE,
  T_GE,
  T_EQ,
  T_NE,
  T_AND,
  T_OR
};

enum UnOpType {
  T_UMINUS,
  T_NOT
};

enum ExprType {
  T_CONST_NAT = 0,
  T_CONST_BINOP,
  T_CONST_UNOP,
  T_VAR,
  T_FUN_APP,
  T_FUN_ABS
};

struct expr {
  enum ExprType t;
  union {
    struct {unsigned int value; } CONST_NAT;
    struct {enum BinOpType op; } CONST_BINOP;
    struct {enum UnOpType op; } CONST_UNOP;
    struct {char * name; } VAR;
    struct {struct expr * left; struct expr * right; } FUN_APP;
    struct {char * name; struct expr * arg; } FUN_ABS;
  } d;
};

struct expr * TConstNat(unsigned int value);
struct expr * TConstBinOp(enum BinOpType op);
struct expr * TConstUnOP(enum UnOpType op);
struct expr * TVar(char * name);
struct expr * TFunApp(struct expr * left, struct expr * right);
struct expr * TFunAbs(char * name, struct expr * arg);

void print_binop(enum BinOpType op);
void print_expr(struct expr * e);
void print_unop(enum UnOpType op);

#endif // LANG_H_INCLUDED
