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

struct checkResult check(struct expr * root, bool inner){

    static unordered_map<string, stack<struct type *> > var_table;
    if(!inner) var_table.clear();

    struct checkResult res;
    res.output = nullptr;

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
            auto func = check(root -> d.FUN_APP.left, true);
            auto var = check(root -> d.FUN_APP.right, true);

            if (func.input.empty()) {
                if (func.output != nullptr && func.output -> t == DT_FUNC) {
                    func.input.push_back(func.output -> d.input);
                    func.output = func.output -> d.output;
                } else {
                    DeleteResult(func);
                    DeleteResult(var);
                    res.success = false;
                    return res;
                }
            }

            struct type * var_type;
            if (var.input.size() == 1) {
                var_type = TPFunc(var.input[0], var.output);
            } else if (var.input.size() >= 2) {
                int n = var.input.size() - 1;
                var_type = TPFunc(var.input[n - 1], var.input[n]);
                for (int i = n - 2; i > 0; i--) {
                    var_type = TPFunc(var.input[i], var_type);
                }
                var_type = TPFunc(var_type, var.output);
            } else var_type = var.output;

            var.input.clear();
            var.output = var_type;
            
            if (!func.success || !var.success || !TypeComp(func.input[0], var.output)) {
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

            res.input = func.input;
            res.input.push_front(CopyType(root -> d.FUN_ABS.typ));
            res.output = func.output;
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