#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "sexpr.h"
#include "lenv.h"

static lval_t* sexpr_ast_to_number(mpc_ast_t *ast) 
{
    if (strstr(ast->contents, ".")) {
        double number = strtod(ast->contents, NULL);
        return errno != ERANGE ? 
            sexpr_lval_fnum_new(number) : 
            sexpr_lval_err_new("number too large %s", ast->contents);
    } else {
        errno = 0; // macro
        long number = strtol(ast->contents, NULL, 10);
        return errno != ERANGE ? 
            sexpr_lval_inum_new(number) : 
            sexpr_lval_err_new("number too large %s", ast->contents);
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
    val->value.integer = num;

    return val;
}

lval_t* sexpr_lval_fnum_new(double num) 
{
    lval_t* val = malloc(sizeof(lval_t));
    val->type = LVAL_TYPE_FNUM;
    val->value.floating_point = num;

    return val;
}

lval_t* sexpr_lval_sym_new(char* sym)
{
    lval_t* val = malloc(sizeof(lval_t));
    val->type = LVAL_TYPE_SYM;
    val->value.symbol = malloc(strlen(sym) + 1);
    strcpy(val->value.symbol, sym);

    return val;
}

lval_t* sexpr_lval_funptr_new(lbuiltin funptr)
{
    lval_t *val = malloc(sizeof(lval_t));
    val->type = LVAL_TYPE_FUN;
    val->value.funptr = funptr;

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

lval_t* sexpr_lval_err_new(char* fmt, ...) 
{
    lval_t* val = malloc(sizeof(lval_t));
    val->type = LVAL_TYPE_ERR;

    // format string and store result in lval
    va_list va;
    va_start(va, fmt);
    val->value.error_message = malloc(512 * sizeof(char));
    vsnprintf(val->value.error_message, 511, fmt, va);
    val->value.error_message = realloc(val->value.error_message, strlen(val->value.error_message) + 1);
    va_end(va);

    return val;
}

void sexpr_lval_free(lval_t* val)
{
    switch (val->type) {
        case LVAL_TYPE_SYM:
            free(val->value.symbol); 
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

    // take memory block from cell[i+1] to cells.count-1 and copy it
    // to memory block starting on cell[i] (shift the whole block one to the left)
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

lval_t* sexpr_lval_join(lval_t *a, lval_t *b) 
{
    while(b->cells.count > 0) {
        a = sexpr_lval_push(a, sexpr_lval_pop(b, 0));
    }

    sexpr_lval_free(b);

    return a;
}

lval_t* sexpr_lval_copy(lval_t *lval)
{
    lval_t *copy = malloc(sizeof(lval_t));
    copy->type = lval->type;

    switch(lval->type) {
        case LVAL_TYPE_INUM: copy->value.integer = lval->value.integer; break;
        case LVAL_TYPE_FNUM: copy->value.floating_point = lval->value.floating_point; break;
        case LVAL_TYPE_FUN: copy->value.funptr = lval->value.funptr; break;
        case LVAL_TYPE_SYM:
            copy->value.symbol = malloc(strlen(lval->value.symbol) + 1);
            strcpy(copy->value.symbol, lval->value.symbol);
            break;
        case LVAL_TYPE_ERR:
            copy->value.error_message = malloc(strlen(lval->value.error_message) + 1);
            strcpy(copy->value.error_message, lval->value.error_message);
            break;
        
        case LVAL_TYPE_SEXPR:
        case LVAL_TYPE_QEXPR:
            copy->cells.count = lval->cells.count;
            copy->cells.cell = malloc(sizeof(lval_t*) * lval->cells.count);
            for (int i = 0; i < lval->cells.count; i++) {
                copy->cells.cell[i] = sexpr_lval_copy(lval->cells.cell[i]);
            }
            break;
    }

    return copy;
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

char* sexpr_type_to_string(int type) 
{
  switch(type) {
      case LVAL_TYPE_INUM: return "Integer number";
      case LVAL_TYPE_FNUM: return "Floating-point number";
      case LVAL_TYPE_SYM: return "Symbol";
      case LVAL_TYPE_FUN: return "Function";
      case LVAL_TYPE_SEXPR: return "S-Expression";
      case LVAL_TYPE_QEXPR: return "Q-Expression";
      case LVAL_TYPE_ERR: return "Error";
      default: return "Unknown";
  }
}
