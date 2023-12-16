#include "interpreter.hpp"
#include <cstring>

void DeleteExpr(struct expr * root) {
    switch(root -> t) {
        case T_FUN_APP: {
            DeleteExpr(root -> d.FUN_APP.left);
            DeleteExpr(root -> d.FUN_APP.right);
            break;
        }
        case T_FUN_ABS:{
            delete [] root -> d.FUN_ABS.name;
            DeleteType(root -> d.FUN_ABS.typ);
            DeleteExpr(root -> d.FUN_ABS.arg);
            break;
        }
        case T_IF_EXPR:{
            DeleteExpr(root -> d.IF_EXPR.cond);
            DeleteExpr(root -> d.IF_EXPR.true_exp);
            DeleteExpr(root -> d.IF_EXPR.false_exp);
            break;
        }
        default: break;
    }
    delete root;
}

interResult simpl (expr * &root) {

    struct interResult res;
    res.type = RT_OTHER;

    switch(root -> t) {
        case T_CONST_NAT: {
            res.type = RT_NAT;
            res.info.NAT.num = root -> d.CONST_NAT.value;
            return res;
        }
        case T_VAR: {
            res.type = RT_OTHER;
            return res;
        }
        case T_CONST_BINOP: {
            res.type = RT_BINOP;
            res.info.BINOP.op = root -> d.CONST_BINOP.op;
            return res;
        }
        case T_CONST_UNOP: {
            res.type = RT_UNOP;
            res.info.UNOP.op = root -> d.CONST_UNOP.op;
            return res;
        }
        case T_FUN_APP: {
            auto func = simpl(root -> d.FUN_APP.left);
            auto var = simpl(root -> d.FUN_APP.right);

            switch (func.type)
            {
            case RT_BINOPL:
                if (var.type == RT_NAT) {
                    res.type = RT_NAT;
                    res.info.NAT.num = ComputeBinOp(func.info.BINOPL.lnum, var.info.NAT.num, func.info.BINOPL.op);
                    DeleteExpr(root);
                    root = TConstNat(res.info.NAT.num);
                }
                break;
            case RT_BINOP:
                if (var.type == RT_NAT) {
                    res.type = RT_BINOPL;
                    res.info.BINOPL.lnum = var.info.NAT.num;
                    res.info.BINOPL.op = func.info.BINOP.op;
                }
                break;
            case RT_UNOP:
                if (var.type == RT_NAT) {
                    res.type = RT_NAT;
                    res.info.NAT.num = ComputeUnOp(var.info.NAT.num, func.info.UNOP.op);
                    DeleteExpr(root);
                    root = TConstNat(res.info.NAT.num);
                }
                break;
            case RT_ABS:{
                auto left = root -> d.FUN_APP.left -> d.FUN_ABS.arg;
                auto right = root -> d.FUN_APP.right;
                substitute(left, right, func.info.ABS.name);

                // 删除abs这个节点
                delete [] root -> d.FUN_APP.left -> d.FUN_ABS.name;
                DeleteType(root -> d.FUN_APP.left -> d.FUN_ABS.typ); 
                delete root -> d.FUN_APP.left;

                
                DeleteExpr(root -> d.FUN_APP.right); // 删除app右子树，即变量的值，因为在substitute中已经拷贝过了
                delete root; // 删除当前节点

                root = left;
                return simpl(root); // 代入后再次化简
            }
            default:
                break;
            }
            return res;
        }
        case T_FUN_ABS:{
            res.type = RT_ABS;
            res.info.ABS.name = root -> d.FUN_ABS.name;
            return res;
        }
        case T_IF_EXPR:{
            auto cond = simpl(root -> d.IF_EXPR.cond);
            auto true_res = simpl(root -> d.IF_EXPR.true_exp);
            auto false_res = simpl(root -> d.IF_EXPR.false_exp);
            if (cond.type == RT_NAT) {
                if (cond.info.NAT.num) {
                    DeleteExpr(root -> d.IF_EXPR.cond);
                    DeleteExpr(root -> d.IF_EXPR.false_exp);
                    auto tmp = root -> d.IF_EXPR.true_exp;
                    delete root;
                    root = tmp;
                    res = true_res;
                } else {
                    DeleteExpr(root -> d.IF_EXPR.cond);
                    DeleteExpr(root -> d.IF_EXPR.true_exp);
                    auto tmp = root -> d.IF_EXPR.false_exp;
                    delete root;
                    root = tmp;
                    res = false_res;
                }
            }
            return res;
        }
        case T_LET_IN:{

        }
        default: exit(1);
    }
}

void substitute (expr * &func, expr * var, char * name) {
    switch(func -> t) {
        case T_VAR: {
            if (strcmp(name, func -> d.VAR.name) == 0) {
                DeleteExpr(func);
                func = CopyExpr(var);
            }
            break;
        }
        case T_FUN_APP: {
            substitute(func -> d.FUN_APP.left, var, name);
            substitute(func -> d.FUN_APP.right, var, name);
            break;
        }
        case T_FUN_ABS:{
            if (strcmp(name, func -> d.FUN_ABS.name) != 0) {
                substitute(func -> d.FUN_ABS.arg, var, name);
            }
            break;
        }
        case T_IF_EXPR:{
            substitute(func -> d.IF_EXPR.cond, var, name);
            substitute(func -> d.IF_EXPR.true_exp, var, name);
            substitute(func -> d.IF_EXPR.false_exp, var, name);
            break;
        }
        case T_LET_IN:{
            
        }
        default: break;
    }
}

int ComputeBinOp(int num1, int num2, BinOpType op) {
    switch (op) {
    case T_PLUS:
        return num1 + num2;
    case T_MINUS:
        return num1 - num2;
    case T_MUL:
        return num1 * num2;
    case T_DIV:
        return num1 / num2;
    case T_MOD:
        return num1 % num2;
    case T_LT:
        return num1 < num2;
    case T_GT:
        return num1 > num2;
    case T_LE:
        return num1 <= num2;
    case T_GE:
        return num1 >= num2;
    case T_EQ:
        return num1 == num2;
    case T_NE:
        return num1 != num2;
    case T_AND:
        return num1 && num2;
    case T_OR:
        return num1 || num2;
    default:
        exit(1);
    }
}

int ComputeUnOp(int num, UnOpType op) {
    switch (op) {
    case T_UMINUS:
        return -num;
    case T_NOT:
        return !num;
    default:
        exit(1);
    }
}

expr * CopyExpr(expr * root) {
    switch(root -> t) {
        case T_CONST_NAT: {
            return TConstNat(root -> d.CONST_NAT.value);
        }
        case T_VAR: {
            return TVar(root -> d.VAR.name);
        }
        case T_CONST_BINOP: {
            return TConstBinOp(root -> d.CONST_BINOP.op);
        }
        case T_CONST_UNOP: {
            return TConstUnOP(root -> d.CONST_UNOP.op);
        }
        case T_FUN_APP: {
            auto left = CopyExpr(root -> d.FUN_APP.left);
            auto right = CopyExpr(root -> d.FUN_APP.right);
            return TFunApp(left, right);
        }
        case T_FUN_ABS:{
            char * name = new char[strlen(root -> d.FUN_ABS.name) + 1];
            strcpy(name, root -> d.FUN_ABS.name);
            auto typ = CopyType(root -> d.FUN_ABS.typ);
            auto arg = CopyExpr(root -> d.FUN_ABS.arg);
            return TFunAbs(name, typ, arg);
        }
        case T_IF_EXPR:{
            auto cond = CopyExpr(root -> d.IF_EXPR.cond);
            auto true_expr = CopyExpr(root -> d.IF_EXPR.true_exp);
            auto false_expr = CopyExpr(root -> d.IF_EXPR.false_exp);
            return TIfExpr(cond, true_expr, false_expr);
        }
        case T_LET_IN:{
            char * name = new char[strlen(root -> d.LET_IN.name) + 1];
            strcpy(name, root -> d.LET_IN.name);
            auto typ = CopyType(root -> d.LET_IN.typ);
            auto substi = CopyExpr(root -> d.LET_IN.expr1);
            auto domain = CopyExpr(root -> d.LET_IN.expr2);
            return TLetIn(name, typ, substi, domain);
        }
        default: exit(1);
    }
}