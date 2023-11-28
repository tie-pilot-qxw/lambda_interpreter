#include "check.hpp"
#include <unordered_map>
#include <stack>
#include <string>

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
}

void DeleteResult(struct checkResult result) {
    DeleteType(result.input);
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

struct checkResult check(struct expr * root, bool inner){

    static unordered_map<string, stack<struct type *> > var_table;
    if(!inner) var_table.clear();

    struct checkResult res;
    res.output = nullptr;
    res.input = nullptr;

    switch(root -> t) {
        case T_CONST_NAT: {
            res.output = TPInt();
            res.success = true;
            return res;
        }
        case T_VAR: {
            auto var = var_table.find(string(root -> d.VAR.name));
            if (var == var_table.end()) {
                printf("undefined variable!\n");
                res.success = false;
                return res;
            }
            auto var_type = var -> second.top();
            res.output = CopyType(var_type);
            res.success = true;
            return res;
        }
        case T_CONST_BINOP: {
            res.output = TPFunc(TPInt(), TPFunc(TPInt(), TPInt()));
            res.success = true;
            return res;
        }
        case T_CONST_UNOP: {
            res.output = TPFunc(TPInt(), TPInt());
            res.success = true;
            return res;
        }
        case T_FUN_APP: {
            auto func = check(root -> d.FUN_APP.left, true);
            auto var = check(root -> d.FUN_APP.right, true);

            if (func.input == nullptr) {
                if (func.output != nullptr && func.output -> t == DT_FUNC) {
                    func.input = func.output -> d.input;
                    func.output = func.output -> d.output;
                } else {
                    DeleteResult(func);
                    DeleteResult(var);
                    res.success = false;
                    return res;
                }
            }

            if (var.input != nullptr) {
                var.output = TPFunc(var.input, var.output);
                var.input = nullptr;
            }
            
            if (!func.success || !var.success || !TypeComp(func.input, var.output)) {
                DeleteResult(func);
                DeleteResult(var);
                res.success = false;
                return res;
            }
            res = func;
            DeleteType(res.input);
            res.input = nullptr;
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
                DeleteResult(func);
                res.success = false;
                return res;
            }

            if (func.input != nullptr) {
                res.output = TPFunc(func.input, func.output);
            } else {
                res.output = func.output;
            }
            res.input = CopyType(root -> d.FUN_ABS.typ);
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

            if (cond.input != nullptr) {
                cond.output = TPFunc(cond.input, cond.output);
                cond.input = nullptr;
            }
            if (true_expr.input != nullptr) {
                true_expr.output = TPFunc(true_expr.input, true_expr.output);
                true_expr.input = nullptr;
            }
            if (false_expr.input != nullptr) {
                false_expr.output = TPFunc(false_expr.input, false_expr.output);
                false_expr.input = nullptr;
            }

            auto type_int = TPInt();
            if (!cond.success || !true_expr.success || !false_expr.success
            || !TypeComp(cond.output, type_int) || !TypeComp(true_expr.output, false_expr.output)) {
                DeleteType(type_int);
                DeleteResult(cond);
                DeleteResult(true_expr);
                DeleteResult(false_expr);
                res.success = false;
                return res;
            }
            DeleteType(type_int);
            
            res = true_expr;
            return res;
        }
        default: exit(1);
    }
}