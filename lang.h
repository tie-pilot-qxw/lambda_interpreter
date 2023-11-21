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

enum DataType {
  DT_INT,
  DT_FUNC
};

struct type {
  enum DataType t;
  struct {struct type * input; struct type * output; } d;
};

enum ExprType {
  T_CONST_NAT = 0,
  T_CONST_BINOP,
  T_CONST_UNOP,
  T_VAR,
  T_FUN_APP,
  T_FUN_ABS,
  T_IF_EXPR,
  T_LET_IN
};

struct expr {
  enum ExprType t;
  union {
    struct {unsigned int value; } CONST_NAT;
    struct {enum BinOpType op; } CONST_BINOP;
    struct {enum UnOpType op; } CONST_UNOP;
    struct {char * name; } VAR;
    struct {struct expr * left; struct expr * right; } FUN_APP;
    struct {char * name; struct type * typ; struct expr * arg; } FUN_ABS;
    struct {struct expr * cond; struct expr * true_exp; struct expr * false_exp; } IF_EXPR;
    struct {char * name; struct type * typ; struct expr * expr1;struct expr * expr2; } LET_IN;
  } d;
};

struct type * TPInt();
struct type * TPFunc(struct type * input, struct type * output);

struct expr * TConstNat(unsigned int value);
struct expr * TConstBinOp(enum BinOpType op);
struct expr * TConstUnOP(enum UnOpType op);
struct expr * TVar(char * name);
struct expr * TFunApp(struct expr * left, struct expr * right);
struct expr * TFunAbs(char * name, struct type * typ, struct expr * arg);
struct expr * TIfExpr(struct expr * cond, struct expr * true_exp, struct expr * false_exp);
struct expr * TLetIn(char * name, struct type * tp, struct expr * expr1, struct expr * expr2);

void print_binop(enum BinOpType op);
void print_expr(struct expr * e);
void print_unop(enum UnOpType op);
void print_type(struct type * t);

#endif // LANG_H_INCLUDED
