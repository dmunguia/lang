#ifndef BUILTINS_QEXPR_H
#define BUILTINS_QEXPR_H

#include "sexpr.h"

#define QEXPR_ASSERT(args, cond, fmt, ...) \
  if (!(cond)) { \
    lval_t* err = sexpr_lval_err_new(fmt, ##__VA_ARGS__); \
    sexpr_lval_free(args); \
    return err; \
  }

lval_t* builtin_qexpr_head(lenv_t* lenv, lval_t *lval);
lval_t* builtin_qexpr_tail(lenv_t* lenv, lval_t *lval);
lval_t* builtin_qexpr_list(lenv_t* lenv, lval_t *lval);
lval_t* builtin_qexpr_eval(lenv_t* lenv, lval_t *lval);
lval_t* builtin_qexpr_join(lenv_t* lenv, lval_t *lval);
lval_t* builtin_qexpr_cons(lenv_t* lenv, lval_t *lval);
lval_t* builtin_qexpr_len(lenv_t* lenv, lval_t *lval);
lval_t* builtin_qexpr_init(lenv_t* lenv, lval_t *lval);

#endif // BUILTINS_QEXPR_H