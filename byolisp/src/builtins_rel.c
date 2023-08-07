#include <stdlib.h>
#include <string.h>

#include "builtins_rel.h"
#include "lenv.h"
#include "sexpr.h"

static lval_t* builtins_rel_eval_operation(char *operator, long a, long b) 
{
    if (strcmp(operator, ">") == 0) { return sexpr_lval_bool_new(a > b); }
    if (strcmp(operator, "<") == 0) { return sexpr_lval_bool_new(a < b); }
    if (strcmp(operator, ">=") == 0) { return sexpr_lval_bool_new(a >= b); }
    if (strcmp(operator, "<=") == 0) { return sexpr_lval_bool_new(a <= b); }

    return sexpr_lval_err_new("unknown function '%s'", operator);
}

static lval_t* builtins_rel_eval_fp_operation(char *operator, double a, double b) 
{
    if (strcmp(operator, ">") == 0) { return sexpr_lval_bool_new(a > b); }
    if (strcmp(operator, "<") == 0) { return sexpr_lval_bool_new(a < b); }
    if (strcmp(operator, ">=") == 0) { return sexpr_lval_bool_new(a >= b); }
    if (strcmp(operator, "<=") == 0) { return sexpr_lval_bool_new(a <= b); }

    return sexpr_lval_err_new("unknown function '%s'", operator);
}

static lval_t* builtins_rel_equality(lval_t *operand_a, lval_t *operand_b)
{
    if (operand_a->type != operand_b->type) {
        return sexpr_lval_bool_new(false);
    }

    switch(operand_a->type) {
        case LVAL_TYPE_INUM: return sexpr_lval_bool_new(operand_a->value.integer == operand_b->value.integer);
        case LVAL_TYPE_FNUM: return sexpr_lval_bool_new(operand_a->value.floating_point == operand_b->value.floating_point);
        case LVAL_TYPE_BOOL: return sexpr_lval_bool_new(operand_a->value.boolean == operand_b->value.boolean);
        case LVAL_TYPE_SYM: return sexpr_lval_bool_new(strcmp(operand_a->value.symbol, operand_b->value.symbol) == 0);
        case LVAL_TYPE_ERR: return sexpr_lval_bool_new(strcmp(operand_a->value.error_message, operand_b->value.error_message) == 0);
        case LVAL_TYPE_FUN: return sexpr_lval_bool_new(operand_a->value.funptr == operand_b->value.funptr);
        case LVAL_TYPE_LAMBDA: 
            return sexpr_lval_bool_new(
                builtins_rel_equality(operand_a->value.lambda->args, operand_b->value.lambda->args)->value.boolean &&
                builtins_rel_equality(operand_a->value.lambda->expr, operand_b->value.lambda->expr)->value.boolean
            );
        
        case LVAL_TYPE_QEXPR:
        case LVAL_TYPE_SEXPR:
            if (operand_a->cells.count != operand_b->cells.count) {
                return sexpr_lval_bool_new(false);
            }
            for (int i = 0; i < operand_a->cells.count; i++) {
                if (!builtins_rel_equality(operand_a->cells.cell[i], operand_b->cells.cell[i])->value.boolean) {
                    return sexpr_lval_bool_new(false);
                }
            }
            return sexpr_lval_bool_new(true);
    }

    return sexpr_lval_bool_new(false);
}

static lval_t* builtins_arith_eval_operator(char *operator, lval_t *operands)
{
    if (operands->cells.count != 2) { // only two operands for relational operators
        sexpr_lval_free(operands);
        return sexpr_lval_err_new("function '%s' expects only two arguments", operator);
    }

    lval_t *operand_a = sexpr_lval_pop(operands, 0);
    lval_t *operand_b = sexpr_lval_pop(operands, 0);

    lval_t *result;

    /** the equality operator applies to any two values: */
    if (strcmp(operator, "=") == 0) {
        result = builtins_rel_equality(operand_a, operand_b);
        sexpr_lval_free(operand_b);
        sexpr_lval_free(operand_a);
        sexpr_lval_free(operands);

        return result;
    }

    /** for the rest of relational operators use the following procedure for numerical values only: **/

    if ((operand_a->type != LVAL_TYPE_INUM && operand_a->type != LVAL_TYPE_FNUM) ||
        (operand_b->type != LVAL_TYPE_INUM && operand_b->type != LVAL_TYPE_FNUM)) {
        sexpr_lval_free(operand_a);
        sexpr_lval_free(operand_b);
        sexpr_lval_free(operands);
        return sexpr_lval_err_new("function '%s' expects numerical arguments", operator);
    }

    // cast long to double when the expression mixes number types
    if (operand_a->type == LVAL_TYPE_FNUM && operand_b->type == LVAL_TYPE_INUM) {
        long ivalue = operand_b->value.integer;
        operand_b->value.floating_point = (double) ivalue;
        operand_b->type = LVAL_TYPE_FNUM;

    } else if (operand_a->type == LVAL_TYPE_INUM && operand_b->type == LVAL_TYPE_FNUM) {
        long ivalue = operand_a->value.integer;
        operand_a->value.floating_point = (double) ivalue;
        operand_a->type = LVAL_TYPE_FNUM;
    }

    if (operand_a->type == LVAL_TYPE_INUM) {
        long a = operand_a->value.integer;
        long b = operand_b->value.integer;

        result = builtins_rel_eval_operation(operator, a, b);
        if (result->type == LVAL_TYPE_ERR) {
            sexpr_lval_free(operand_a);
            sexpr_lval_free(operand_b);
            sexpr_lval_free(operands);
            return result;
        }

    } else {
        double a = operand_a->value.floating_point;
        double b = operand_b->value.floating_point;

        result = builtins_rel_eval_fp_operation(operator, a, b);
        if (result->type == LVAL_TYPE_ERR) {
            sexpr_lval_free(operand_a);
            sexpr_lval_free(operand_b);
            sexpr_lval_free(operands);
            return result;
        }
    }

    sexpr_lval_free(operand_b);
    sexpr_lval_free(operand_a);
    sexpr_lval_free(operands);

    return result;
}

lval_t* builtins_rel_gt(lenv_t* lenv, lval_t* lval)
{
    return builtins_arith_eval_operator(">", lval);
}

lval_t* builtins_rel_lt(lenv_t* lenv, lval_t* lval)
{
    return builtins_arith_eval_operator("<", lval);
}

lval_t* builtins_rel_ge(lenv_t* lenv, lval_t* lval)
{
    return builtins_arith_eval_operator(">=", lval);
}

lval_t* builtins_rel_le(lenv_t* lenv, lval_t* lval)
{
    return builtins_arith_eval_operator("<=", lval);
}

lval_t* builtins_rel_eq(lenv_t* lenv, lval_t* lval)
{
    return builtins_arith_eval_operator("=", lval);
}

