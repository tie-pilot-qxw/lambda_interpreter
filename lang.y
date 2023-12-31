%{
	// this part is copied to the beginning of the parser 
	#include <stdio.h>
	#include "lang.h"
	#include "lexer.h"
	void yyerror(char *);
	int yylex(void);
  struct expr * root;
%}

%union {
unsigned int n;
char * i;
struct expr * e;
struct type * t;
void * none;
}

// Terminals
%token <n> TM_NAT
%token <i> TM_IDENT
%token <none> TM_LEFT_PAREN TM_RIGHT_PAREN
%token <none> TM_LET TM_IN TM_LAMBDA TM_COLON
%token <none> TM_ASGNOP
%token <none> TM_OR
%token <none> TM_AND
%token <none> TM_NOT
%token <none> TM_LT TM_LE TM_GT TM_GE TM_EQ TM_NE
%token <none> TM_PLUS TM_MINUS
%token <none> TM_MUL TM_DIV TM_MOD
%token <none> TM_IF TM_THEN TM_ELSE
%token <none> TM_LEFT_BRACE TM_RIGHT_BRACE
%token <none> TM_INT TM_TO TM_DOT

// Nonterminals
%type <e> NT_WHOLE
%type <e> NT_EXPR0
%type <e> NT_EXPR1
%type <e> NT_EXPR
%type <e> NT_APPLY
%type <t> NT_TYPE0
%type <t> NT_TYPE
%type <e> NT_DEF

// Priority
%right  TM_COLON TM_DOT TM_IN
%nonassoc TM_ASGNOP
%left TM_OR
%left TM_AND
%left TM_LT TM_LE TM_GT TM_GE TM_EQ TM_NE
%left TM_PLUS TM_MINUS
%left TM_MUL TM_DIV TM_MOD
%left TM_NOT
%left TM_LEFT_PAREN TM_RIGHT_PAREN


%%

NT_WHOLE:
  NT_DEF
  {
    $$ = ($1);
    root = $$;
  }
;

NT_TYPE0:
  TM_LEFT_PAREN NT_TYPE TM_RIGHT_PAREN
  {
    $$ = ($2);
  }
| TM_INT
  {
    $$ = TPInt();
  }
;

NT_TYPE:
  NT_TYPE0
  {
    $$ = ($1);
  }
| NT_TYPE0 TM_TO NT_TYPE
  {
    $$ = TPFunc($1, $3);
  }
;

NT_EXPR0:
  TM_NAT
  {
    $$ = (TConstNat($1));
  }
| TM_LEFT_PAREN NT_DEF TM_RIGHT_PAREN
  {
    $$ = ($2);
  }
| TM_IDENT
  {
    $$ = (TVar($1));
  }
;

NT_EXPR1:
  TM_NOT NT_EXPR0
  {
    $$ = (TFunApp(TConstUnOP(T_NOT), $2));
  }
| TM_MINUS NT_EXPR0
  {
    $$ = (TFunApp(TConstUnOP(T_UMINUS), $2));
  }
| NT_EXPR0
  {
    $$ = ($1);
  }
;


NT_EXPR:
  NT_EXPR1
  {
    $$ = ($1);
  }
| NT_EXPR TM_MUL NT_EXPR
  {
    $$ = (TFunApp(TFunApp(TConstBinOp(T_MUL), $1), $3));
  }
| NT_EXPR TM_PLUS NT_EXPR
  {
    $$ = (TFunApp(TFunApp(TConstBinOp(T_PLUS), $1), $3));
  }
| NT_EXPR TM_MINUS NT_EXPR
  {
    $$ = (TFunApp(TFunApp(TConstBinOp(T_MINUS), $1), $3));
  }
| NT_EXPR TM_DIV NT_EXPR
  {
    $$ = (TFunApp(TFunApp(TConstBinOp(T_DIV), $1), $3));
  }
| NT_EXPR TM_MOD NT_EXPR
  {
    $$ = (TFunApp(TFunApp(TConstBinOp(T_MOD), $1), $3));
  }
| NT_EXPR TM_LT NT_EXPR
  {
    $$ = (TFunApp(TFunApp(TConstBinOp(T_LT), $1), $3));
  }
| NT_EXPR TM_GT NT_EXPR
  {
    $$ = (TFunApp(TFunApp(TConstBinOp(T_GT), $1), $3));
  }
| NT_EXPR TM_LE NT_EXPR
  {
    $$ = (TFunApp(TFunApp(TConstBinOp(T_LE), $1), $3));
  }
| NT_EXPR TM_GE NT_EXPR
  {
    $$ = (TFunApp(TFunApp(TConstBinOp(T_GE), $1), $3));
  }
| NT_EXPR TM_EQ NT_EXPR
  {
    $$ = (TFunApp(TFunApp(TConstBinOp(T_EQ), $1), $3));
  }
| NT_EXPR TM_NE NT_EXPR
  {
    $$ = (TFunApp(TFunApp(TConstBinOp(T_NE), $1), $3));
  }
| NT_EXPR TM_AND NT_EXPR
  {
    $$ = (TFunApp(TFunApp(TConstBinOp(T_AND), $1), $3));
  }
| NT_EXPR TM_OR NT_EXPR
  {
    $$ = (TFunApp(TFunApp(TConstBinOp(T_OR), $1), $3));
  }
| NT_APPLY
  {
    $$ = $1;
  }
| TM_IF TM_LEFT_PAREN NT_EXPR TM_RIGHT_PAREN TM_THEN TM_LEFT_BRACE NT_EXPR TM_RIGHT_BRACE TM_ELSE TM_LEFT_BRACE NT_EXPR TM_RIGHT_BRACE
  {
    $$ = TIfExpr($3, $7, $11);
  }
;

NT_APPLY:
  NT_APPLY NT_EXPR0 {
    $$ = TFunApp($1, $2);
  }
| NT_EXPR0 NT_EXPR0
  {
    $$ = TFunApp($1, $2);
  }
;

NT_DEF:
  NT_EXPR {
    $$ = $1;
  }
|
  TM_LAMBDA TM_IDENT TM_COLON NT_TYPE TM_DOT NT_DEF
  {
    $$ = TFunAbs($2, $4, $6);
  }
| TM_LET TM_IDENT TM_COLON NT_TYPE TM_DOT TM_COLON TM_ASGNOP NT_DEF TM_IN NT_DEF
  {
    $$ = TLetIn($2, $4, $8, $10);
  }
;
%%

void yyerror(char* s)
{
    fprintf(stderr , "%s\n",s);
}
                                                                                         