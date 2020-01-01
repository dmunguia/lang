#ifndef BUILTINS_QEXPR_H
#define BUILTINS_QEXPR_H

#include "sexpr.h"

#define QEXPR_ASSERT(args, cond, err) \
  if (!(cond)) { sexpr_lval_free(args); return sexpr_lval_err_new(err); }

lval_t* builtin_qexpr_head(lval_t *lval);
lval_t* builtin_qexpr_tail(lval_t *lval);
lval_t* builtin_qexpr_list(lval_t *lval);
lval_t* builtin_qexpr_eval(lval_t *lval);
lval_t* builtin_qexpr_join(lval_t *lval);
lval_t* builtin_qexpr_cons(lval_t *lval);
lval_t* builtin_qexpr_len(lval_t *lval);
lval_t* builtin_qexpr_init(lval_t *lval);

#endif // BUILTINS_QEXPR_H