#ifndef BUILTINS_ARITH_H
#define BUILTINS_ARITH_H

#include "lenv.h"
#include "sexpr.h"

lval_t* builtins_arith_add(lenv_t* lenv, lval_t* lval);
lval_t* builtins_arith_sub(lenv_t* lenv, lval_t* lval);
lval_t* builtins_arith_mul(lenv_t* lenv, lval_t* lval);
lval_t* builtins_arith_div(lenv_t* lenv, lval_t* lval);
lval_t* builtins_arith_mod(lenv_t* lenv, lval_t* lval);
lval_t* builtins_arith_pow(lenv_t* lenv, lval_t* lval);
lval_t* builtins_arith_min(lenv_t* lenv, lval_t* lval);
lval_t* builtins_arith_max(lenv_t* lenv, lval_t* lval);
lval_t* builtins_arith_neg(lenv_t* lenv, lval_t* lval);

#endif // BUILTINS_ARITH_H