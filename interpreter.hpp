#pragma once

#include "check.hpp"

extern "C"{
    #include "lang.h"
}

enum resultType {
    RT_NAT,
    RT_BINOPL,
    RT_BINOP,
    RT_UNOP,
    RT_ABS,
    RT_OTHER,
};

struct interResult {
    resultType type;
    union {
        struct {int num;} NAT;
        struct {BinOpType op; int lnum; } BINOPL;
        struct {BinOpType op;} BINOP;
        struct {UnOpType op;} UNOP;
        struct {char* name;} ABS;
    } info;
};

interResult simpl (expr * &root); // 替换后，新的语法树还在root里
void substitute (expr * &func, expr * var, char * name); // 替换后，新的语法树存在func里

void DeleteExpr(expr * root);
int ComputeBinOp(int num1, int num2, BinOpType op);
int ComputeUnOp(int num, UnOpType op);
expr * CopyExpr(expr * root);