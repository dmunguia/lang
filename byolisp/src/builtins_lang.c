#include "builtins_lang.h"
#include "builtins_qexpr.h"
#include "lenv.h"
#include "sexpr.h"

lval_t* builtins_lang_def(lenv_t* lenv, lval_t* lval)
{
    QEXPR_ASSERT(lval, lval->cells.cell[0]->type == LVAL_TYPE_QEXPR, 
        "Function 'def' passed incorrect arguments. Mismatching data type, expecting %s, got %s", 
        sexpr_type_to_string(LVAL_TYPE_QEXPR), sexpr_type_to_string(lval->cells.cell[0]->type));

    lval_t* symbols = lval->cells.cell[0];

    for (int i = 0; i < symbols->cells.count; i++) {
        QEXPR_ASSERT(lval, symbols->cells.cell[i]->type == LVAL_TYPE_SYM, 
            "Function 'def' passed incorrect arguments. Mismatching data type, expecting %s, got %s", 
            sexpr_type_to_string(LVAL_TYPE_SYM), sexpr_type_to_string(symbols->cells.cell[i]->type));
    }

    QEXPR_ASSERT(lval, symbols->cells.count == lval->cells.count-1, 
        "Function 'def' passed too many arguments. Expecting %i, got %i",
        lval->cells.count-1, symbols->cells.count);

    for (int i = 0; i < symbols->cells.count; i++) {
        lenv_put(lenv, symbols->cells.cell[i]->value.symbol, lval->cells.cell[i+1]);
    }

    sexpr_lval_free(lval);

    return sexpr_lval_sexpr_new();
}