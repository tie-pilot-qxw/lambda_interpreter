#include "check.hpp"
#include <unordered_map>
#include <stack>
#include <string>
#include <iostream>

using std::unordered_map, std::stack, std::string;
using std::make_pair;

void DeleteType(struct type * typ) {
    if (typ == nullptr) return; 
    if (typ -> t == DT_INT) delete typ;
    else {
        DeleteType(typ -> d.input);
        DeleteType(typ -> d.output);
        delete typ;
    }
    typ = nullptr;
}

struct type * CopyType(struct type * typ) {
    if (typ -> t == DT_INT) {
        return TPInt();
    } else {
        return TPFunc(CopyType(typ -> d.input), CopyType(typ -> d.output));
    }
}

bool TypeComp(const struct type * type1, const struct type * type2 ){
    if (type1 == nullptr || type2 == nullptr) return false;
    if (type1 -> t != type2 -> t) return false;
    if (type1 -> t == DT_INT) return true;
    return TypeComp(type1 -> d.input, type2 -> d.input) && TypeComp(type1 -> d.output, type2 -> d.output);
}

struct checkResult check(struct expr * root, bool inner){

    static unordered_map<string, stack<struct type *> > var_table;
    if(!inner) var_table.clear();

    struct checkResult res;
    res.t = nullptr;
    res.success = false;

    switch(root -> t) {
        case T_CONST_NAT: {
            res.t = TPInt();
            res.success = true;
            return res;
        }
        case T_VAR: {
            auto var = var_table.find(string(root -> d.VAR.name));
            if (var == var_table.end()) {
                printf("undefined variable!\n");
                return res;
            }
            auto var_type = var -> second.top();
            res.t = CopyType(var_type);
            res.success = true;
            return res;
        }
        case T_CONST_BINOP: {
            res.t = TPFunc(TPInt(), TPFunc(TPInt(), TPInt()));
            res.success = true;
            return res;
        }
        case T_CONST_UNOP: {
            res.t = TPFunc(TPInt(), TPInt());
            res.success = true;
            return res;
        }
        case T_FUN_APP: {
            auto func = check(root -> d.FUN_APP.left, true);
            auto var = check(root -> d.FUN_APP.right, true);

            if (func.t == nullptr || func.t -> t == DT_INT) {
                DeleteType(func.t);
                DeleteType(var.t);
                return res;
            }
            
            if (!func.success || !var.success || !TypeComp(func.t -> d.input, var.t)) {
                DeleteType(func.t);
                DeleteType(var.t);
                return res;
            }

            DeleteType(var.t);
            DeleteType(func.t -> d.input);
            res.t = func.t -> d.output;
            res.success = true;
            return res;
        }
        case T_FUN_ABS:{
            auto var = var_table.find(string(root -> d.FUN_ABS.name));
            if (var != var_table.end()) {
                var -> second.push(root -> d.FUN_ABS.typ);
            } else {
                stack<type *> tmp;
                tmp.push(root -> d.FUN_ABS.typ);
                var_table.insert(make_pair(string(root -> d.FUN_ABS.name), tmp));
            }
            
            auto func = check(root -> d.FUN_ABS.arg, true);
            if (!func.success) {
                DeleteType(func.t);
                return res;
            }

            res.t = TPFunc(CopyType(root -> d.FUN_ABS.typ), func.t);
            res.success = true;

            var = var_table.find(string(root -> d.FUN_ABS.name));
            if(var == var_table.end()) {
                printf("name stack error in check process");
                exit(1);
            }
            var -> second.pop();

            return res;
        }
        case T_IF_EXPR:{
            auto cond = check(root -> d.IF_EXPR.cond, true);
            auto true_expr = check(root -> d.IF_EXPR.true_exp, true);
            auto false_expr = check(root -> d.IF_EXPR.false_exp, true);

            auto type_int = TPInt();
            if (!cond.success || !true_expr.success || !false_expr.success
            || !TypeComp(cond.t, type_int) || !TypeComp(true_expr.t, false_expr.t)) {
                DeleteType(type_int);
                DeleteType(cond.t);
                DeleteType(true_expr.t);
                DeleteType(false_expr.t);
                return res;
            }
            DeleteType(type_int);
            DeleteType(cond.t);
            DeleteType(false_expr.t);
            
            res = true_expr;
            return res;
        }
        case T_LET_IN:{
            auto var = var_table.find(string(root -> d.LET_IN.name));
            if (var != var_table.end()) {
                var -> second.push(root -> d.LET_IN.typ);
            } else {
                stack<type *> tmp;
                tmp.push(root -> d.LET_IN.typ);
                var_table.insert(make_pair(string(root -> d.LET_IN.name), tmp));
            }
            
            auto substi = check(root -> d.LET_IN.expr1,true);
            auto domain = check(root -> d.LET_IN.expr2,true);
            
            if (!substi.success || !domain.success || !TypeComp(substi.t,root -> d.LET_IN.typ)){
                DeleteType(substi.t);
                DeleteType(domain.t);
                return res;
            }
            var = var_table.find(string(root -> d.LET_IN.name));
            var -> second.pop();
            res = domain;
            return res;
        }
        default: exit(1);
    }
}