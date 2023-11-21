%option noyywrap yylineno
%option outfile="lexer.c" header-file="lexer.h"
%{
#include "lang.h"
#include "parser.h"
%}

%%

0|[1-9][0-9]* {
    yylval.n = build_nat(yytext, yyleng);
    return TM_NAT;
}

"if" {
    return TM_IF;
}

"then" {
    return TM_THEN;
}

"else" {
    return TM_ELSE;
}

"{" {
    return TM_LEFT_BRACE;
}

"}" {
    return TM_RIGHT_BRACE;
}

"int" {
    return TM_INT;
}

"->" {
    return TM_TO;
}

"." {
    return TM_DOT;
}

"lambda" {
    return TM_LAMBDA;
}

"let" {
    return TM_LET;
}

"in" {
    return TM_IN;
}

":" {
    return TM_COLON;
}

[_A-Za-z][_A-Za-z0-9]* {
    yylval.i = new_str(yytext, yyleng);
    return TM_IDENT;
}

"(" {
    return TM_LEFT_PAREN;
    }

")" {
    return TM_RIGHT_PAREN;
    }

"+" {
    return TM_PLUS;
    }

"-" {
    return TM_MINUS;
    }

"*" {
    return TM_MUL;
    }

"/" {
    return TM_DIV;
    }

"%" {
    return TM_MOD;
    }

"<" {
    return TM_LT;
    }

">" {
    return TM_GT;
    }

"<=" {
    return TM_LE;
    }

">=" {
    return TM_GE;
    }

"==" {
    return TM_EQ;
    }

"!=" {
    return TM_NE;
    }

"=" {
    return TM_ASGNOP;
    }

"&&" {
    return TM_AND;
    }

"||" {
    return TM_OR;
    }

"!" {
    return TM_NOT;
    }

[ \t\n\r]    { };

.   {printf("%s",yytext);
     return -1; }
%%
