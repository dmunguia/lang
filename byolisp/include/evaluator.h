#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "lenv.h"
#include "sexpr.h"

lval_t* evaluator_evaluate(lenv_t* lenv, lval_t* lval);

#endif // EVALUATOR_H