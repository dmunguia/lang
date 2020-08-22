#include <stdbool.h>
#include "builtins_lang.h"
#include "builtins_qexpr.h"
#include "lenv.h"
#include "sexpr.h"

static lval_t* builtins_lang_bindings(lenv_t* lenv, lval_t* lval, char* func)
{
    QEXPR_ASSERT_BUILTIN_TYPES(func, lval, lval->cells.cell[0]->type, LVAL_TYPE_QEXPR);

    lval_t* symbols = lval->cells.cell[0];

    for (int i = 0; i < symbols->cells.count; i++) {
        QEXPR_ASSERT_BUILTIN_TYPES(func, lval, symbols->cells.cell[i]->type, LVAL_TYPE_SYM);
    }

    QEXPR_ASSERT_BUILTIN_NARGS(func, lval, lval->cells.count-1, symbols->cells.count);

    for (int i = 0; i < symbols->cells.count; i++) {
        bool success;
        if (strcmp(func, "def") == 0) {
            success = lenv_def(lenv, symbols->cells.cell[i]->value.symbol, lval->cells.cell[i+1]);
        } else if (strcmp(func, "<-") == 0) {
            success = lenv_put(lenv, symbols->cells.cell[i]->value.symbol, lval->cells.cell[i+1]);
        }

        if (!success) {
            return sexpr_lval_err_new("Cannot redefine builtin function '%s'", symbols->cells.cell[i]->value.symbol);
        }
    }

    sexpr_lval_free(lval);

    return sexpr_lval_sexpr_new();
}

lval_t* builtins_lang_bind_global(lenv_t* lenv, lval_t* lval)
{
    return builtins_lang_bindings(lenv, lval, "def");
}

lval_t* builtins_lang_bind_local(lenv_t* lenv, lval_t* lval)
{
    return builtins_lang_bindings(lenv, lval, "<-");
}

lval_t* builtins_lang_lambda(lenv_t* lenv, lval_t* lval)
{
    QEXPR_ASSERT_BUILTIN_NARGS("\\", lval, lval->cells.count, 2);
    QEXPR_ASSERT_BUILTIN_TYPES("\\", lval, lval->cells.cell[0]->type, LVAL_TYPE_QEXPR);
    QEXPR_ASSERT_BUILTIN_TYPES("\\", lval, lval->cells.cell[1]->type, LVAL_TYPE_QEXPR);

    for (int i = 0; i < lval->cells.cell[0]->cells.count; i++) {
        QEXPR_ASSERT_BUILTIN_TYPES("\\", lval, lval->cells.cell[0]->cells.cell[i]->type, LVAL_TYPE_SYM);
    }

    lval_t* args = sexpr_lval_pop(lval, 0);
    lval_t* expr = sexpr_lval_pop(lval, 0);

    sexpr_lval_free(lval);
    
    return sexpr_lval_lambda_new(args, expr);
}

lval_t* builtins_lang_call(lenv_t* lenv, lval_t* lambda, lval_t* lval)
{
    int given = lval->cells.count;
    int required = lambda->value.lambda->args->cells.count;

    while (lval->cells.count > 0) {

        if (lambda->value.lambda->args->cells.count == 0) {
            sexpr_lval_free(lval);
            return sexpr_lval_err_new("Function passed too many arguments. Received %i, expected %i", given, required);
        }

        lval_t* symbol = sexpr_lval_pop(lambda->value.lambda->args, 0);
        lval_t* value = sexpr_lval_pop(lval, 0);

        lenv_put(lambda->value.lambda->lenv, symbol->value.symbol, value);
        sexpr_lval_free(symbol);
        sexpr_lval_free(value);
    }

    sexpr_lval_free(lval);

    if (lambda->value.lambda->args->cells.count == 0) {
        lambda->value.lambda->lenv->parent = lenv;

        return builtin_qexpr_eval(
            lambda->value.lambda->lenv,
            sexpr_lval_push(sexpr_lval_sexpr_new(), sexpr_lval_copy(lambda->value.lambda->expr))
        );
    } else {
        return sexpr_lval_copy(lambda); // curried function
    }
}
