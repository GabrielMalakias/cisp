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

int number_of_nodes(mpc_ast_t* ast) {
  if (ast->children_num == 0) {
    return 1;
  } else {
    int total = 1;
    for(int i = 0; i < ast->children_num; i++) {
      total = total + number_of_nodes(ast->children[i]);
    }
    return total;
  }
}

long eval_op(long x, char* operator, long y) {
  if (strcmp(operator, "+") == 0) { return x + y; }
  if (strcmp(operator, "-") == 0) { return x - y; }
  if (strcmp(operator, "*") == 0) { return x * y; }
  if (strcmp(operator, "/") == 0) { return x / y; }
  return 0;
}

long eval(mpc_ast_t* ast) {
  if (strstr(ast->tag, "number")) {
    return atoi(ast->contents);
  }

  char* operator = ast->children[1]->contents;

  long result = eval(ast->children[2]);

  int index = 3;
  while(strstr(ast->children[index]->tag, "expr")) {
    result = eval_op(result, operator, eval(ast->children[index]));
    index++;
  }

  return result;
}

int main(int argc, char** argv) {
  mpc_parser_t* Number     = mpc_new("number");
  mpc_parser_t* Operator   = mpc_new("operator");
  mpc_parser_t* Expr       = mpc_new("expr");
  mpc_parser_t* Cisp       = mpc_new("cisp");

  mpca_lang(MPCA_LANG_DEFAULT,
      "                                                                                    \
        number      : /-?[0-9]+/ ;                                                          \
        operator    : '+' | '-' | '*' | '/' ;                                               \
        expr        : <number> | '(' <operator> <expr>+ ')' ;        \
        cisp        : /^/ <operator> <expr>+ /$/ ;                                          \
      ",
      Number, Operator, Expr, Cisp);

  printf("Cisp version %s\n", version);
  puts("Press Ctrl+c to Exit\n");


  while(1) {
    char* input = readline("ƒ>> ");
    add_history(input);

    mpc_result_t result;
    if(mpc_parse("<stdin>", input, Cisp, &result)) {
      printf("= %li\n", eval(result.output));

      mpc_ast_delete(result.output);
    } else {
      mpc_err_print(result.error);
      mpc_err_delete(result.error);
    }

    free(input);
  }

  mpc_cleanup(5, Number, Operator, Expr, Cisp);
  return 0;
}
