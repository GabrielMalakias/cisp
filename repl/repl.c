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

enum { LVAL_NUM, LVAL_ERR };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

typedef struct {
  int type;
  long num;
  int err;
} lval;

lval lval_num(long x){
  lval value;
  value.type = LVAL_NUM;
  value.num = x;
  return value;
}

lval lval_err(int x) {
  lval value;
  value.type = LVAL_ERR;
  value.err = x;
  return value;
}

void lval_print_err(lval err) {
  switch(err.err) {
    case LERR_DIV_ZERO: printf("ERROR: Division by Zero"); break;
    case LERR_BAD_OP:   printf("ERROR: Invalid Operator"); break;
    case LERR_BAD_NUM:  printf("ERROR: Invalid number!!"); break;
  }
}

void lval_print(lval value) {
  switch(value.type) {
    case LVAL_NUM: printf("%li", value.num); break;
    case LVAL_ERR: lval_print_err(value); break;
  }
}

void lval_println(lval v) { lval_print(v); putchar('\n'); }

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

int max(int x, int y) {
  return (x < y) ? y : x;
}

int min(int x, int y) {
  return (x > y) ? y : x;
}

lval eval_op(lval x, char* operator, lval y) {
  if (x.type == LVAL_ERR) { return x; }
  if (y.type == LVAL_ERR) { return y; }

  if (strcmp(operator, "+") == 0) { return lval_num(x.num + y.num); }
  if (strcmp(operator, "-") == 0) { return lval_num(x.num - y.num); }
  if (strcmp(operator, "*") == 0) { return lval_num(x.num * y.num); }
  if (strcmp(operator, "%") == 0) { return lval_num(x.num % y.num); }
  if (strcmp(operator, "^") == 0) { return lval_num(pow(x.num, y.num)); }
  if (strcmp(operator, "min") == 0) { return lval_num(min(x.num, y.num)); }
  if (strcmp(operator, "max") == 0) { return lval_num(max(x.num, y.num)); }
  if (strcmp(operator, "/") == 0) {
    return y.num == 0
      ? lval_err(LERR_DIV_ZERO)
      : lval_num(x.num / y.num);
  }
  return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* ast) {
  if (strstr(ast->tag, "number")) {
    errno = 0;
    long x = strtol(ast->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  char* operator = ast->children[1]->contents;

  lval result = eval(ast->children[2]);

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
      number      : /-?[0-9]+/ ;                                                         \
      operator    : '+' | '-' | '*' | '/' | '%' | '^' | /min/ | /max/ ;                  \
      expr        : <number> | '(' <operator> <expr>+ ')' ;                              \
      cisp        : /^/ <operator> <expr>+ /$/ ;                                         \
      ",
      Number, Operator, Expr, Cisp);

  printf("Cisp version %s\n", version);
  puts("Press Ctrl+c to Exit\n");


  while(1) {
    char* input = readline("ƒ>> ");
    add_history(input);

    mpc_result_t result;
    if(mpc_parse("<stdin>", input, Cisp, &result)) {
      lval_println(eval(result.output));

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
