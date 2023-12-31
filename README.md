# Lambda Interpreter

## 要求

lambda 表达式的解释器 这个任务中的 lambda 表达式包括 lambda 函数抽象（带有类型标注）、函数应用、算术运算、 大小比较、布尔运算与 let 表达式。

- [x] 要求 1：完成 lambda 表达式的词法分析、语法分析与语法 树输出。
- [x] 要求 2：实现 lambda 表达式的类型检查。
- [x] 要求 3：解释执行 lambda 表达式。

## 语法树

```c
Whole   ::= Def
Def     ::= Expr | "lambda" IDENT ":" Type "." Def |
        "let" IDENT ":" Type "." ":=" Def "in" Def
Expr    ::= Expr1 | Expr BinOp Expr | Apply |
        "if" "(" Expr ")" "then" "{" Expr "}" "else" "{" Expr "}"
Type    ::= Type0 | Type0 "->" Type
Type0   ::= "(" TYpe ")" | "int"
Expr1   ::= Expr0 | "!" Expr0 | "-" Expr0
Expr0   ::= NAT | IDENT | "(" Def ")"
BinOp   ::= "+" | "-" | "*" | "/" | "%" | ">" | "<" | ">=" | "<=" |
        "==" | "!=" | "&&" | "||"
```
