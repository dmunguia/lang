#include <stdlib.h>
#include <string.h>

#include "sexpr.h"

static lval_t* sexpr_ast_to_number(mpc_ast_t *ast) 
{
    if (strstr(ast->contents, ".")) {
        double number = strtod(ast->contents, NULL);
        return errno != ERANGE ? 
            sexpr_lval_fnum_new(number) : 
            sexpr_lval_err_new(LVAL_ERR_NUM_TOO_LARGE);
    } else {
        errno = 0; // macro
        long number = strtol(ast->contents, NULL, 10);
        return errno != ERANGE ? 
            sexpr_lval_inum_new(number) : 
            sexpr_lval_err_new(LVAL_ERR_NUM_TOO_LARGE);
    }
}

static lval_t* sexpr_ast_to_symbol(mpc_ast_t *ast)
{
    return sexpr_lval_sym_new(ast->contents);
}

lval_t* sexpr_lval_inum_new(long num) 
{
    lval_t* val = malloc(sizeof(lval_t));
    val->type = LVAL_TYPE_INUM;
    val->value.integer_value = num;

    return val;
}

lval_t* sexpr_lval_fnum_new(double num) 
{
    lval_t* val = malloc(sizeof(lval_t));
    val->type = LVAL_TYPE_FNUM;
    val->value.double_value = num;

    return val;
}

lval_t* sexpr_lval_sym_new(char* sym)
{
    lval_t* val = malloc(sizeof(lval_t));
    val->type = LVAL_TYPE_SYM;
    val->value.symbol_value = malloc(strlen(sym) + 1);
    strcpy(val->value.symbol_value, sym);

    return val;
}

lval_t* sexpr_lval_sexpr_new(void)
{
    lval_t* val = malloc(sizeof(lval_t));
    val->type = LVAL_TYPE_SEXPR;
    val->cells.count = 0;
    val->cells.cell = NULL;

    return val;
}

lval_t* sexpr_lval_qexpr_new(void)
{
    lval_t* val = malloc(sizeof(lval_t));
    val->type = LVAL_TYPE_QEXPR;
    val->cells.count = 0;
    val->cells.cell = NULL;

    return val;
}

lval_t* sexpr_lval_err_new(int err_code) 
{
    lval_t* val = malloc(sizeof(lval_t));
    val->type = LVAL_TYPE_ERR;
    val->err_code = err_code;

    return val;
}

void sexpr_lval_free(lval_t* val)
{
    switch (val->type) {
        case LVAL_TYPE_SYM:
            free(val->value.symbol_value); 
            break;
        
        case LVAL_TYPE_SEXPR:
        case LVAL_TYPE_QEXPR:
            for (int i = 0; i < val->cells.count; i++) {
                sexpr_lval_free(val->cells.cell[i]);
            }

            free(val->cells.cell);
            break;
        
        default:
            break;
    }

    free(val);
}

lval_t* sexpr_lval_push(lval_t *lval, lval_t *new)
{
    lval->cells.count++;
    lval->cells.cell = realloc(lval->cells.cell, sizeof(lval_t*) * lval->cells.count);
    lval->cells.cell[lval->cells.count - 1] = new;
    return lval;
}

lval_t* sexpr_lval_pop(lval_t *lval, int i)
{
    lval_t *ith = lval->cells.cell[i];

    memmove(&lval->cells.cell[i], &lval->cells.cell[i+1],
        sizeof(lval_t*) * (lval->cells.count-i-1));

    lval->cells.count--;

    lval->cells.cell = realloc(lval->cells.cell, sizeof(lval_t*) * lval->cells.count);

    return ith;
}

lval_t* sexpr_lval_take(lval_t *lval, int i)
{
    lval_t *ith = sexpr_lval_pop(lval, i);
    sexpr_lval_free(lval);

    return ith;
}

lval_t* sexpr_lval_join(lval_t *a, lval_t *b) {
    while(b->cells.count > 0) {
        a = sexpr_lval_push(a, sexpr_lval_pop(b, 0));
    }

    sexpr_lval_free(b);

    return a;
}

lval_t* sexpr_build_from_ast(mpc_ast_t *ast)
{
    if (strstr(ast->tag, "number")) {
        return sexpr_ast_to_number(ast);
    }

    if (strstr(ast->tag, "symbol")) {
        return sexpr_ast_to_symbol(ast);
    }

    lval_t *lval = NULL;
    if (strcmp(ast->tag, ">") == 0) { // ROOT
        lval = sexpr_lval_sexpr_new();
    }

    if (strstr(ast->tag, "sexpr")) {
        lval = sexpr_lval_sexpr_new();
    }

    if (strstr(ast->tag, "qexpr")) {
        lval = sexpr_lval_qexpr_new();
    }

    for (int i = 0; i < ast->children_num; i++) {
        if ((strcmp(ast->children[i]->contents, "(") != 0) &&
            (strcmp(ast->children[i]->contents, ")") != 0) &&
            (strcmp(ast->children[i]->contents, "{") != 0) &&
            (strcmp(ast->children[i]->contents, "}") != 0) &&
            (strcmp(ast->children[i]->tag, "regex") != 0)) {
            lval = sexpr_lval_push(lval, sexpr_build_from_ast(ast->children[i]));
        }
    }

    return lval;
}
