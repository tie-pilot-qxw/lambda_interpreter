#include "check.hpp"
#include <unordered_map>
#include <stack>

using std::unordered_map, std::stack;
using std::make_pair;

void DeleteType(struct type * typ) {
    if (typ -> t == DT_INT) delete typ;
    else {
        DeleteType(typ -> d.input);
        DeleteType(typ -> d.output);
        delete typ;
    }
}

void DeleteResult(struct checkResult result) {
    for(int i = 0; i < result.input.size(); i++) {
        DeleteType(result.input[i]);
    }
    DeleteType(result.output);
}

struct type * CopyType(struct type * typ) {
    if (typ -> t == DT_INT) {
        return TPInt();
    } else {
        return TPFunc(CopyType(typ -> d.input), CopyType(typ -> d.output));
    }
}

bool TypeComp(const struct type * type1, const struct type * type2 ){
    if (type1 -> t != type2 -> t) return false;
    if (type1 -> t == DT_INT) return true;
    return TypeComp(type1 -> d.input, type2 -> d.input) && TypeComp(type1 -> d.output, type2 -> d.output);
}

struct checkResult check(struct expr * root){

    static unordered_map<char *, stack<struct type *> > var_table;

    struct checkResult res;
    res.output = nullptr;

    switch(root -> t) {
        case T_CONST_NAT: {
            res.output = TPInt();
            res.success = true;
            return res;
        }
        case T_VAR: {
            auto var_type = var_table.find(root -> d.VAR.name);
            if (var_type == var_table.end()) {
                printf("undefined variable!\n");
                res.success = false;
                return res;
            }
            res.output = CopyType(var_type -> second.top());
            res.success = true;
            return res;
        }
        case T_CONST_BINOP: {
            res.input.push_back(TPInt());
            res.input.push_back(TPInt());
            res.output = TPInt();
            res.success = true;
            return res;
        }
        case T_CONST_UNOP: {
            res.input.push_back(TPInt());
            res.output = TPInt();
            res.success = true;
            return res;
        }
        case T_FUN_APP: {
            auto func = check(root -> d.FUN_APP.left);
            auto var = check(root -> d.FUN_APP.right);

            if (var.input.size() == 1) {
                var.output = TPFunc(var.input[0], var.output);
            } else if (var.input.size() >= 2) {
                struct type * tmp = TPFunc(var.input[0], var.input[1]);
                for (int i = 2; i < var.input.size(); i++) {
                    tmp = TPFunc(tmp, var.input[i]);
                }
                var.output = TPFunc(tmp, var.output);
            }
            
            if (!func.success || !var.success || func.input.empty()
            || !TypeComp(func.input[0], var.output)) {
                DeleteResult(func);
                DeleteResult(var);
                res.success = false;
                return res;
            }
            res.input = func.input;
            DeleteType(res.input[0]);
            res.input.pop_front();
            res.output = func.output;
            res.success = true;
            return res;
        }
        case T_FUN_ABS:{
            auto var = var_table.find(root -> d.FUN_ABS.name);
            if (var != var_table.end()) {
                var -> second.push(root -> d.FUN_ABS.typ);
            } else {
                stack<type *> tmp;
                tmp.push(root -> d.FUN_ABS.typ);
                var_table.insert(make_pair(root -> d.FUN_ABS.name, tmp));
            }
            
            auto func = check(root -> d.FUN_ABS.arg);
            if (!func.success) {
                DeleteResult(func);
                res.success = false;
                return res;
            }

            res.input = func.input;
            res.input.push_front(CopyType(root -> d.FUN_ABS.typ));
            res.output = func.output;
            res.success = true;

            var = var_table.find(root -> d.FUN_ABS.name);
            if(var == var_table.end()) {
                printf("name stack error in check process");
                exit(1);
            }
            var -> second.pop();

            return res;
        }
        case T_IF_EXPR:{
            auto cond = check(root -> d.IF_EXPR.cond);
            auto true_expr = check(root -> d.IF_EXPR.true_exp);
            auto false_expr = check(root -> d.IF_EXPR.false_exp);
            
            auto type_int = TPInt();
            if (!cond.success || !true_expr.success || !false_expr.success
            || !cond.input.empty() || !TypeComp(cond.output, type_int)
            || !TypeComp(true_expr.output, false_expr.output)
            || true_expr.input.size() != false_expr.input.size()) {
                DeleteResult(cond);
                DeleteResult(true_expr);
                DeleteResult(false_expr);
                res.success = false;
                return res;
            }
            DeleteType(type_int);
            
            for (int i = 0; i < true_expr.input.size(); i++) {
                if (!TypeComp(true_expr.input[i], false_expr.input[i])){
                    DeleteResult(cond);
                    DeleteResult(true_expr);
                    DeleteResult(false_expr);
                    res.success = false;
                    return res;
                }
            }
            
            res = true_expr;
            return res;
        }
        default: exit(1);
    }
}