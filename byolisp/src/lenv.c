#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "builtins_arith.h"
#include "builtins_lang.h"
#include "builtins_qexpr.h"
#include "lenv.h"
#include "sexpr.h"

lenv_t* lenv_new()
{
    lenv_t* lenv = malloc(sizeof(lenv_t));
    lenv->parent = NULL;
    lenv->count = 0;
    lenv->symbols = NULL;
    lenv->values = NULL;
    lenv->is_builtin = NULL;

    return lenv;
}

void lenv_free(lenv_t* lenv)
{
    for (int i = 0; i < lenv->count; i++) {
        free(lenv->symbols[i]);
        sexpr_lval_free(lenv->values[i]);
    }

    free(lenv->symbols);
    free(lenv->values);
    free(lenv->is_builtin);
    free(lenv);
}

lval_t* lenv_get(lenv_t* lenv, char* key) 
{    
    bool found = false;
    lval_t* copy = NULL;
    for (int i = 0; i < lenv->count && !found; i++) {
        if (strcmp(lenv->symbols[i], key) == 0) {
            copy = sexpr_lval_copy(lenv->values[i]);
            found = true;
        }
    }

    if (found) {
        return copy;
    } else if (lenv->parent) {
        return lenv_get(lenv->parent, key);
    } else {
        return sexpr_lval_err_new("unbound symbol '%s'", key);
    }
}

bool lenv_put(lenv_t* lenv, char* key, lval_t* value)
{
    bool found = false;
    for (int i = 0; i < lenv->count && !found; i++) {
        if (strcmp(lenv->symbols[i], key) == 0) {
            if (lenv->is_builtin[i]) {
                return false;
            }

            sexpr_lval_free(lenv->values[i]);
            lenv->values[i] = sexpr_lval_copy(value);            
            found = true;
        }
    }

    if (!found) {
        lenv->count++;
        lenv->values = realloc(lenv->values, sizeof(lval_t*) * lenv->count);
        lenv->symbols = realloc(lenv->symbols, sizeof(char*) * lenv->count);
        lenv->is_builtin = realloc(lenv->is_builtin, sizeof(bool*) * lenv->count);

        lenv->values[lenv->count - 1] = sexpr_lval_copy(value);
        lenv->symbols[lenv->count - 1] = malloc(strlen(key) + 1);
        strcpy(lenv->symbols[lenv->count - 1], key);
        lenv->is_builtin[lenv->count - 1] = false;
    }

    return true;
}

bool lenv_def(lenv_t* lenv, char* key, lval_t* value)
{
    while (lenv->parent) {
        lenv = lenv->parent;
    }

    return lenv_put(lenv, key, value);
}

lenv_t* lenv_copy(lenv_t* lenv)
{
    lenv_t* copy = malloc(sizeof(lenv_t));
    copy->parent = lenv->parent;
    copy->count = lenv->count;
    copy->symbols = malloc(sizeof(char*) * copy->count);
    copy->values = malloc(sizeof(lval_t*) * copy->count);
    copy->is_builtin = malloc(sizeof(bool) * copy->count);

    for (int i = 0; i < lenv->count; i++) {
        copy->symbols[i] = malloc(strlen(lenv->symbols[i]) + 1);
        strcpy(copy->symbols[i], lenv->symbols[i]);
        copy->values[i] = sexpr_lval_copy(lenv->values[i]);
        copy->is_builtin[i] = lenv->is_builtin[i];
    }

    return copy;
}

static void lenv_builtin_put(lenv_t* lenv, char* key, lval_t* value)
{
    lenv->count++;
    lenv->values = realloc(lenv->values, sizeof(lval_t*) * lenv->count);
    lenv->symbols = realloc(lenv->symbols, sizeof(char*) * lenv->count);
    lenv->is_builtin = realloc(lenv->is_builtin, sizeof(bool*) * lenv->count);

    lenv->values[lenv->count - 1] = sexpr_lval_copy(value);
    lenv->symbols[lenv->count - 1] = malloc(strlen(key) + 1);
    strcpy(lenv->symbols[lenv->count - 1], key);
    lenv->is_builtin[lenv->count - 1] = true;
}

static void lenv_add_builtin(lenv_t* lenv, char* name, lbuiltin funptr)
{
    lval_t* value = sexpr_lval_funptr_new(funptr);
    lenv_builtin_put(lenv, name, value);
    sexpr_lval_free(value);
}

void lenv_add_builtins(lenv_t* lenv)
{
    lenv_add_builtin(lenv, "list", builtin_qexpr_list);
    lenv_add_builtin(lenv, "head", builtin_qexpr_head);
    lenv_add_builtin(lenv, "tail", builtin_qexpr_tail);
    lenv_add_builtin(lenv, "eval", builtin_qexpr_eval);
    lenv_add_builtin(lenv, "join", builtin_qexpr_join);
    lenv_add_builtin(lenv, "cons", builtin_qexpr_cons);
    lenv_add_builtin(lenv, "len", builtin_qexpr_len);
    lenv_add_builtin(lenv, "init", builtin_qexpr_init);

    lenv_add_builtin(lenv, "+", builtins_arith_add);
    lenv_add_builtin(lenv, "-", builtins_arith_sub);
    lenv_add_builtin(lenv, "*", builtins_arith_mul);
    lenv_add_builtin(lenv, "/", builtins_arith_div);
    lenv_add_builtin(lenv, "%", builtins_arith_mod);
    lenv_add_builtin(lenv, "^", builtins_arith_pow);
    lenv_add_builtin(lenv, "min", builtins_arith_min);
    lenv_add_builtin(lenv, "max", builtins_arith_max);
    lenv_add_builtin(lenv, "neg", builtins_arith_neg);

    lenv_add_builtin(lenv, "def", builtins_lang_bind_global);
    lenv_add_builtin(lenv, "\\", builtins_lang_lambda);
    lenv_add_builtin(lenv, "<-", builtins_lang_bind_local);
}