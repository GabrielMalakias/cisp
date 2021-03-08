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

enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

typedef struct lval {
  int type;
  long num;

  char* err;
  char* sym;

  int count;
  struct lval** cell;
} lval;

lval* lval_num(long x){
  lval* value = malloc(sizeof(lval));

  value->type = LVAL_NUM;
  value->num = x;

  return value;
}

lval* lval_err(char* m) {
  lval* value = malloc(sizeof(lval));

  value->type = LVAL_ERR;
  value->err = malloc(strlen(m) + 1);
  strcpy(value->err, m);

  return value;
}

lval* lval_sym(char* s) {
  lval* value = malloc(sizeof(lval));

  value->type = LVAL_SYM;
  value->sym = malloc(strlen(s) + 1);
  strcpy(value->sym, s);

  return value;
}

lval* lval_sexpr(void) {
  lval* value = malloc(sizeof(lval));

  value->type = LVAL_SEXPR;
  value->count = 0;
  value->cell = NULL;

  return value;
}

void lval_print(lval* v);

void lval_expr_print(lval* value, char open, char close) {
  putchar(open);
  for (int i = 0; i < value->count; i++) {
    lval_print(value->cell[i]);

    if (i != (value->count - 1)) {
      putchar(' ');
    }
  }
  putchar(close);
}

lval* lval_add(lval* value, lval* new) {
  value->count++;
  value->cell = realloc(value->cell, sizeof(lval*) * value->count);
  value->cell[value->count - 1] = new;

  return value;
}

lval* lval_read_num(mpc_ast_t* node) {
  errno = 0;
  long x = strtol(node->contents, NULL, 10);

  return errno != ERANGE ? lval_num(x) : lval_err("Invalid Number");
}

lval* lval_read(mpc_ast_t* tree) {
  if (strstr(tree->tag, "number")) { return lval_read_num(tree); }
  if (strstr(tree->tag, "symbol")) { return lval_sym(tree->contents); }

  lval* x = NULL;
  if (strcmp(tree->tag, ">") == 0) { x = lval_sexpr(); }
  if (strstr(tree->tag, "sexpr"))  { x = lval_sexpr(); }

  for(int i = 0; i < tree->children_num; i++) {
    if (strcmp(tree->children[i]->contents, "(") == 0)     { continue; }
    if (strcmp(tree->children[i]->contents, ")") == 0)     { continue; }
    if (strcmp(tree->children[i]->contents, "{") == 0)     { continue; }
    if (strcmp(tree->children[i]->contents, "}") == 0)     { continue; }
    if (strcmp(tree->children[i]->tag,  "regex") == 0)     { continue; }

    x = lval_add(x, lval_read(tree->children[i]));
  }

  return x;
}


void lval_del(lval* value) {
  switch(value->type) {
    case LVAL_NUM: break;
    case LVAL_ERR: free(value->err); break;
    case LVAL_SYM: free(value->sym); break;
    case LVAL_SEXPR:
       for(int i = 0; i < value->count; i++){
         lval_del(value->cell[i]);
       }

       free(value->cell);
    break;
  }

  free(value);
}

void lval_print(lval* value) {
  switch(value->type) {
    case LVAL_NUM:   printf("%li", value->num); break;
    case LVAL_ERR:   printf("Error: %s", value->err); break;
    case LVAL_SYM:   printf("%s", value->sym); break;
    case LVAL_SEXPR: lval_expr_print(value, '(', ')'); break;
  }
}

void lval_println(lval* v) { lval_print(v); putchar('\n'); }

int max(int x, int y) {
  return (x < y) ? y : x;
}

int min(int x, int y) {
  return (x > y) ? y : x;
}

/*
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
*/

int main(int argc, char** argv) {
  mpc_parser_t* Number     = mpc_new("number");
  mpc_parser_t* Symbol     = mpc_new("symbol");
  mpc_parser_t* Sexpr      = mpc_new("sexpr");
  mpc_parser_t* Expr       = mpc_new("expr");
  mpc_parser_t* Cisp       = mpc_new("cisp");

  mpca_lang(MPCA_LANG_DEFAULT,
      "                                                                                    \
      number      : /-?[0-9]+/ ;                                                         \
      symbol      : '+' | '-' | '*' | '/' | '%' | '^' | /min/ | /max/ ;                  \
      sexpr       : '(' <expr>* ')' ;                                                     \
      expr        : <number> | <symbol> | <sexpr> ;                                        \
      cisp        : /^/ <expr>+ /$/ ;                                         \
      ",
      Number, Symbol, Sexpr, Expr, Cisp);

  printf("Cisp version %s\n", version);
  puts("Press Ctrl+c to Exit\n");


  while(1) {
    char* input = readline("ƒ>> ");
    add_history(input);

    mpc_result_t result;
    if(mpc_parse("<stdin>", input, Cisp, &result)) {
      lval* ast = lval_read(result.output);
      lval_println(ast);
      lval_del(ast);

      mpc_ast_delete(result.output);
    } else {
      mpc_err_print(result.error);
      mpc_err_delete(result.error);
    }

    free(input);
  }

  mpc_cleanup(5, Number, Symbol, Expr, Sexpr, Cisp);
  return 0;
}
