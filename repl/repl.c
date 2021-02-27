/*
 * =====================================================================================
 *
 *       Filename:  repl.c
 *
 *    Description:  Basic repl
 *
 *        Version:  1.0
 *        Created:  02/27/21 12:26:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

#include <editline/readline.h>

static char* version = "0.0.0";

/*  Loops waiting for the input */

int main(int argc, char** argv) {
  mpc_parser_t* Integer    = mpc_new("integer");
  mpc_parser_t* Decimal    = mpc_new("decimal");
  mpc_parser_t* String     = mpc_new("string");
  mpc_parser_t* Operator   = mpc_new("operator");
  mpc_parser_t* Expr       = mpc_new("expr");
  mpc_parser_t* Cisp       = mpc_new("cisp");

  mpca_lang(MPCA_LANG_DEFAULT,
      "                                                                                    \
        integer     : /-?[0-9]+/;                                                          \
        decimal     : /-?[0-9]+.[0-9]+/;                                                 \
        string      : /\"[a-z]+\"/;                                                        \
        operator    : '+' | '-' | '*' | '/' | '%';                                         \
        expr        : <string> | <decimal> | <integer> | '(' <operator> <expr>+ ')';        \
        cisp        : /^/ <operator> <expr>+ /$/;                                          \
      ",
      Integer, Decimal, String, Operator, Expr, Cisp);

  printf("Cisp version %s\n", version);
  puts("Press Ctrl+c to Exit\n");


  while(1) {
    char* input = readline("ƒ>> ");
    add_history(input);

    mpc_result_t result;
    if(mpc_parse("<stdin>", input, Cisp, &result)) {
      mpc_ast_print(result.output);
      mpc_ast_delete(result.output);
    } else {
      mpc_err_print(result.error);
      mpc_err_delete(result.error);
    }

    free(input);
  }

  mpc_cleanup(6, Integer, Decimal, String, Operator, Expr, Cisp);
  return 0;
}
