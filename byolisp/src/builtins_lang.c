#include "builtins_lang.h"
#include "builtins_qexpr.h"
#include "lenv.h"
#include "sexpr.h"

lval_t* builtins_lang_def(lenv_t* lenv, lval_t* lval)
{
    QEXPR_ASSERT(lval, lval->cells.cell[0]->type == LVAL_TYPE_QEXPR, LVAL_ERR_MISMATCH_DATATYPE);

    lval_t* symbols = lval->cells.cell[0];

    for (int i = 0; i < symbols->cells.count; i++) {
        QEXPR_ASSERT(lval, symbols->cells.cell[i]->type == LVAL_TYPE_SYM, LVAL_ERR_MISMATCH_DATATYPE);
    }

    QEXPR_ASSERT(lval, symbols->cells.count == lval->cells.count-1, LVAL_ERR_INCORRECT_NUM_ARGS);

    for (int i = 0; i < symbols->cells.count; i++) {
        lenv_put(lenv, symbols->cells.cell[i]->value.symbol_value, lval->cells.cell[i+1]);
    }

    sexpr_lval_free(lval);

    return sexpr_lval_sexpr_new();
}