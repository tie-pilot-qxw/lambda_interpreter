#include <stdio.h>

extern "C" {
  #include "lang.h"
  #include "lexer.h"
  #include "parser.h"
}

#include "check.hpp"
#include "interpreter.hpp"

extern struct expr * root;
int yyparse();

int main(int argc, char * * argv) {
  if (argc == 1) {
    printf("Error, not enough arguments!\n");
    return 0;
  }
  if (argc >= 3) {
    printf("Error, too many arguments!\n");
    return 0;
  }
  yyin = fopen(argv[1], "rb");
  if (yyin == NULL) {
    printf("File %s can't be opened.\n", argv[1]);
    return 0;
  }
  yyparse();
  fclose(yyin);
  print_expr(root);
  printf("\n");
  auto result = check(root);
  if (result.success) {
    printf("type check passed!\n");
    // simpl(root);
    // print_expr(root);
  } else {
    printf("type check failed!\n");
  }
  DeleteType(result.t);
  DeleteExpr(root);
  return 0;
}
