#ifndef BUILTINS_LANG_H
#define BUILTINS_LANG_H

#include "lenv.h"
#include "sexpr.h"

lval_t* builtins_lang_def(lenv_t* lenv, lval_t* lval);

#endif // BUILTINS_LANG_H