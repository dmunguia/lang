#ifndef BUILTINS_LANG_H
#define BUILTINS_LANG_H

#include "lenv.h"
#include "sexpr.h"

lval_t* builtins_lang_bind_global(lenv_t* lenv, lval_t* lval);
lval_t* builtins_lang_bind_local(lenv_t* lenv, lval_t* lval);
lval_t* builtins_lang_lambda(lenv_t* lenv, lval_t* lval);
lval_t* builtins_lang_call(lenv_t* lenv, lval_t* lambda, lval_t* lval);
lval_t* builtins_lang_fun(lenv_t* lenv, lval_t* lval);

#endif // BUILTINS_LANG_H