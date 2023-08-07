#ifndef BUILTINS_REL_H
#define BUILTINS_REL_H

#include "lenv.h"
#include "sexpr.h"

lval_t* builtins_rel_gt(lenv_t* lenv, lval_t* lval);
lval_t* builtins_rel_lt(lenv_t* lenv, lval_t* lval);
lval_t* builtins_rel_ge(lenv_t* lenv, lval_t* lval);
lval_t* builtins_rel_le(lenv_t* lenv, lval_t* lval);
lval_t* builtins_rel_eq(lenv_t* lenv, lval_t* lval);

#endif // BUILTINS_REL_H