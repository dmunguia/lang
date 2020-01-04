#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "builtins_qexpr.h"
#include "evaluator.h"
#include "sexpr.h"

static lval_t* evaluator_evaluate_sexpr(lenv_t* lenv, lval_t* lval)
{
    for (int i = 0; i < lval->cells.count; i++) {
        lval->cells.cell[i] = evaluator_evaluate(lenv, lval->cells.cell[i]);
    }

    for (int i = 0; i < lval->cells.count; i++) { // error checking
        if (lval->cells.cell[i]->type == LVAL_TYPE_ERR) {
            return sexpr_lval_take(lval, i);
        }
    }

    if (lval->cells.count == 0) { // empty expression
        return lval;
    }

    if (lval->cells.count == 1) { // single expression
        return sexpr_lval_take(lval, 0);
    }

    lval_t *funptr = sexpr_lval_pop(lval, 0);
    if (funptr->type != LVAL_TYPE_FUN) {
        sexpr_lval_free(funptr);
        sexpr_lval_free(lval);
        return sexpr_lval_err_new(LVAL_ERR_NOT_A_FUNCTION);
    }

    lval_t *result = funptr->value.funptr_value(lenv, lval);
    sexpr_lval_free(funptr);

    return result;
}

lval_t* evaluator_evaluate(lenv_t* lenv, lval_t* lval)
{
    lval_t* evaluated = NULL;

    switch(lval->type) {
        case LVAL_TYPE_SYM:
            evaluated = lenv_get(lenv, lval->value.symbol_value);
            sexpr_lval_free(lval);
            break;
        case LVAL_TYPE_SEXPR:
            evaluated = evaluator_evaluate_sexpr(lenv, lval);
            break;
        default:
            evaluated = lval;
    }

    return evaluated;
}
