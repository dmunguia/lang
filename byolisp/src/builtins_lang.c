#include <stdbool.h>
#include "builtins_lang.h"
#include "builtins_qexpr.h"
#include "lenv.h"
#include "sexpr.h"

lval_t* builtins_lang_def(lenv_t* lenv, lval_t* lval)
{
    QEXPR_ASSERT_BUILTIN_TYPES("def", lval, lval->cells.cell[0]->type, LVAL_TYPE_QEXPR);

    lval_t* symbols = lval->cells.cell[0];

    for (int i = 0; i < symbols->cells.count; i++) {
        QEXPR_ASSERT_BUILTIN_TYPES("def", lval, symbols->cells.cell[i]->type, LVAL_TYPE_SYM);
    }

    QEXPR_ASSERT_BUILTIN_NARGS("def", lval, lval->cells.count-1, symbols->cells.count);

    for (int i = 0; i < symbols->cells.count; i++) {
        bool success = lenv_put(lenv, symbols->cells.cell[i]->value.symbol, lval->cells.cell[i+1]);
        if (!success) {
            return sexpr_lval_err_new("Cannot redefine builtin function '%s'", symbols->cells.cell[i]->value.symbol);
        }
    }

    sexpr_lval_free(lval);

    return sexpr_lval_sexpr_new();
}