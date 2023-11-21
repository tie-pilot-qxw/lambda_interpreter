#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lang.h"

struct expr * new_expr_ptr() {
  struct expr * res = (struct expr *) malloc(sizeof(struct expr));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  return res;
}

struct type * new_type_ptr() {
  struct type * res = (struct type *) malloc(sizeof(struct type));
  if (res == NULL) {
    printf("Failure in malloc.\n");
    exit(0);
  }
  return res;
}

struct type * TPInt() {
  struct type * res = new_type_ptr();
  res -> t = DT_INT;
  return res;
}

struct type * TPFunc(struct type * input, struct type * output) {
  struct type * res = new_type_ptr();
  res -> t = DT_FUNC;
  res -> d.input = input;
  res -> d.output = output;
  return res;
}

struct expr * TLetIn(char * name, struct type * typ, struct expr * expr1, struct expr * expr2) {
  struct expr * res = new_expr_ptr();
  res -> t = T_LET_IN;
  res -> d.LET_IN.typ = typ;
  res -> d.LET_IN.name = name;
  res -> d.LET_IN.expr1 = expr1;
  res -> d.LET_IN.expr2 = expr2;
}

struct expr * TConstNat(unsigned int value) {
  struct expr * res = new_expr_ptr();
  res -> t = T_CONST_NAT;
  res -> d.CONST_NAT.value = value;
  return res;
}

struct expr * TVar(char * name) {
  struct expr * res = new_expr_ptr();
  res -> t = T_VAR;
  res -> d.VAR.name = name;
  return res;
}

struct expr * TConstBinOp(enum BinOpType op) {
  struct expr * res = new_expr_ptr();
  res -> t = T_CONST_BINOP;
  res -> d.CONST_BINOP.op = op;
  return res;
}

struct expr * TConstUnOP(enum UnOpType op) {
  struct expr * res = new_expr_ptr();
  res -> t = T_CONST_UNOP;
  res -> d.CONST_UNOP.op = op;
  return res;
}

struct expr * TFunApp(struct expr * left, struct expr * right) {
  struct expr * res = new_expr_ptr();
  res -> t = T_FUN_APP;
  res -> d.FUN_APP.left = left;
  res -> d.FUN_APP.right = right;
  return res;
}

struct expr * TFunAbs(char * name, struct type * typ, struct expr * arg) {
  struct expr * res = new_expr_ptr();
  res -> t = T_FUN_ABS;
  res -> d.FUN_ABS.name = name;
  res -> d.FUN_ABS.typ = typ;
  res -> d.FUN_ABS.arg = arg;
  return res;
}

struct expr * TIfExpr(struct expr * cond, struct expr * true_exp, struct expr * false_exp) {
  struct expr * res = new_expr_ptr();
  res -> t = T_IF_EXPR;
  res -> d.IF_EXPR.cond = cond;
  res -> d.IF_EXPR.true_exp = true_exp;
  res -> d.IF_EXPR.false_exp = false_exp;
  return res;
}

void print_binop(enum BinOpType op) {
  switch (op) {
  case T_PLUS:
    printf("PLUS");
    break;
  case T_MINUS:
    printf("MINUS");
    break;
  case T_MUL:
    printf("MUL");
    break;
  case T_DIV:
    printf("DIV");
    break;
  case T_MOD:
    printf("MOD");
    break;
  case T_LT:
    printf("LT");
    break;
  case T_GT:
    printf("GT");
    break;
  case T_LE:
    printf("LE");
    break;
  case T_GE:
    printf("GE");
    break;
  case T_EQ:
    printf("EQ");
    break;
  case T_NE:
    printf("NE");
    break;
  case T_AND:
    printf("AND");
    break;
  case T_OR:
    printf("OR");
    break;
  }
}

void print_type(struct type * t) {
  if (t -> t == DT_INT) {
    printf("TYPE(int)");
  } else {
    printf("TYPE(");
    print_type(t -> d.input);
    printf("->");
    print_type(t -> d.output);
    printf(")");
  }
}

void print_unop(enum UnOpType op) {
  switch (op) {
  case T_UMINUS:
    printf("UMINUS");
    break;
  case T_NOT:
    printf("NOT");
    break;
  }
}

void print_expr(struct expr * e) {
  switch (e -> t) {
  case T_CONST_NAT:
    printf("CONST_NAT(%d)", e -> d.CONST_NAT.value);
    break;
  case T_VAR:
    printf("VAR(%s)", e -> d.VAR.name);
    break;
  case T_CONST_BINOP:
    printf("CONST_BINOP(");
    print_binop(e -> d.CONST_BINOP.op);
    printf(")");
    break;
  case T_CONST_UNOP:
    printf("CONST_UNOP(");
    print_unop(e -> d.CONST_UNOP.op);
    printf(")");
    break;
  case T_FUN_APP:
    printf("FUN_APP(");
    print_expr(e -> d.FUN_APP.left);
    printf(",");
    print_expr(e -> d.FUN_APP.right);
    printf(")");
    break;
  case T_FUN_ABS:
    printf("FUN_ABS(%s,", e -> d.FUN_ABS.name);
    print_type(e -> d.FUN_ABS.typ);
    printf(",");
    print_expr(e -> d.FUN_ABS.arg);
    printf(")");
    break;
  case T_IF_EXPR:
    printf("IF(");
    print_expr(e -> d.IF_EXPR.cond);
    printf(",");
    print_expr(e -> d.IF_EXPR.true_exp);
    printf(",");
    print_expr(e -> d.IF_EXPR.true_exp);
    printf(")");
    break;
  case T_LET_IN:
    printf("LET(VAR(%s",e -> d.LET_IN.name);
    printf("),");
    print_type(e -> d.LET_IN.typ);
    printf(",");
    print_expr(e -> d.LET_IN.expr1);
    printf(",");
    print_expr(e -> d.LET_IN.expr2);
    printf(")");
    break;
  }
}


