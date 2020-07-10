#include "builtins.h"
#include "builtins_arith.h"
#include "lenv.h"
#include "sexpr.h"

static lval_t* builtins_arith_eval_integer_operation(char *operator, long a, long b) 
{

    if (strcmp(operator, "+") == 0) { return sexpr_lval_inum_new(a + b); }
    if (strcmp(operator, "-") == 0) { return sexpr_lval_inum_new(a - b); }
    if (strcmp(operator, "*") == 0) { return sexpr_lval_inum_new(a * b); }
    if (strcmp(operator, "%") == 0) { return sexpr_lval_inum_new(a % b); }
    if (strcmp(operator, "min") == 0) { return sexpr_lval_inum_new(builtin_imin(a, b)); }
    if (strcmp(operator, "max") == 0) { return sexpr_lval_inum_new(builtin_imax(a, b)); }
    if (strcmp(operator, "neg") == 0) { return sexpr_lval_inum_new(-1 * a); }

    if (strcmp(operator, "/") == 0) { 
        return b == 0 ?
            sexpr_lval_err_new("division by zero") :
            sexpr_lval_inum_new(a / b);
    }

    if (strcmp(operator, "^") == 0) { 
        return b < 0 ?
            sexpr_lval_err_new("exponent can't be negative") :
            sexpr_lval_inum_new(builtin_ipow(a, b)); 
    }

    return sexpr_lval_err_new("unknown function '%s'", operator);
}

static lval_t* builtins_arith_eval_fp_operation(char *operator, double a, double b) 
{

    if (strcmp(operator, "+") == 0) { return sexpr_lval_fnum_new(a + b); }
    if (strcmp(operator, "-") == 0) { return sexpr_lval_fnum_new(a - b); }
    if (strcmp(operator, "*") == 0) { return sexpr_lval_fnum_new(a * b); }
    if (strcmp(operator, "%") == 0) { return sexpr_lval_fnum_new(fmod(a, b)); }
    if (strcmp(operator, "min") == 0) { return sexpr_lval_fnum_new(builtin_fmin(a, b)); }
    if (strcmp(operator, "max") == 0) { return sexpr_lval_fnum_new(builtin_fmax(a, b)); }
    if (strcmp(operator, "neg") == 0) { return sexpr_lval_fnum_new(-1 * a); }

    if (strcmp(operator, "/") == 0) { 
        return b == 0 ?
            sexpr_lval_err_new("division by zero") :
            sexpr_lval_fnum_new(a / b);
    }

    if (strcmp(operator, "^") == 0) { 
        return b < 0 ?
            sexpr_lval_err_new("exponent can't be negative") :
            sexpr_lval_fnum_new(builtin_fpow(a, b)); 
    }

    return sexpr_lval_err_new("unknown function '%s'", operator);
}

static lval_t* builtins_arith_eval_operator(char *operator, lval_t *operands)
{
    for (int i = 0; i < operands->cells.count; i++) { // all data types must be numeric
        if (operands->cells.cell[i]->type != LVAL_TYPE_INUM && operands->cells.cell[i]->type != LVAL_TYPE_FNUM) {
            sexpr_lval_free(operands);
            return sexpr_lval_err_new("function '%s' expects numerical arguments", operator);
        }
    }

    lval_t *operand_a = sexpr_lval_pop(operands, 0);

    if ((strcmp(operator, "-") == 0) && operands->cells.count == 0) {
        if (operand_a->type == LVAL_TYPE_INUM) {
            operand_a->value.integer = -(operand_a->value.integer);
        } else if (operand_a->type == LVAL_TYPE_FNUM) {
            operand_a->value.floating_point = -(operand_a->value.floating_point);
        } else { // this should de impossible due to previous type check
            sexpr_lval_free(operand_a);
            sexpr_lval_free(operands);
            return sexpr_lval_err_new("unknown function '%s'", operator);
        }
    }

    while (operands->cells.count > 0) {
        lval_t *operand_b = sexpr_lval_pop(operands, 0);

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

            lval_t *result = builtins_arith_eval_integer_operation(operator, a, b);
            if (result->type == LVAL_TYPE_ERR) {
                sexpr_lval_free(operand_a);
                sexpr_lval_free(operand_b);
                sexpr_lval_free(operands);
                return result;
            } else {
                operand_a->value.integer = result->value.integer;
                sexpr_lval_free(result);
            }

        } else {
            double a = operand_a->value.floating_point;
            double b = operand_b->value.floating_point;

            lval_t *result = builtins_arith_eval_fp_operation(operator, a, b);
            if (result->type == LVAL_TYPE_ERR) {
                sexpr_lval_free(operand_a);
                sexpr_lval_free(operand_b);
                sexpr_lval_free(operands);
                return result;
            } else {
                operand_a->value.floating_point = result->value.floating_point;
                sexpr_lval_free(result);
            }
        }

        sexpr_lval_free(operand_b);
    }

    sexpr_lval_free(operands);
    return operand_a;
}

static lval_t* builtins_arith_eval_function(char *func, lval_t *lval) 
{
    lval_t *result = NULL;
    if (strcmp("min", func) == 0 ||
        strcmp("max", func) == 0 ||
        strstr("+-*/%^", func)) {
        result = builtins_arith_eval_operator(func, lval);
    }

    if (result == NULL) {
        result = sexpr_lval_err_new("unknown function '%s'", func);
        sexpr_lval_free(lval);
    }

    return result;
}

lval_t* builtins_arith_add(lenv_t* lenv, lval_t* lval)
{
    return builtins_arith_eval_function("+", lval);
}

lval_t* builtins_arith_sub(lenv_t* lenv, lval_t* lval)
{
    return builtins_arith_eval_function("-", lval);
}

lval_t* builtins_arith_mul(lenv_t* lenv, lval_t* lval)
{
    return builtins_arith_eval_function("*", lval);
}

lval_t* builtins_arith_div(lenv_t* lenv, lval_t* lval)
{
    return builtins_arith_eval_function("/", lval);
}

lval_t* builtins_arith_mod(lenv_t* lenv, lval_t* lval)
{
    return builtins_arith_eval_function("%", lval);
}

lval_t* builtins_arith_pow(lenv_t* lenv, lval_t* lval)
{
    return builtins_arith_eval_function("^", lval);
}

lval_t* builtins_arith_min(lenv_t* lenv, lval_t* lval)
{
    return builtins_arith_eval_function("min", lval);
}

lval_t* builtins_arith_max(lenv_t* lenv, lval_t* lval)
{
    return builtins_arith_eval_function("max", lval);
}

lval_t* builtins_arith_neg(lenv_t* lenv, lval_t* lval)
{
    return builtins_arith_eval_function("-", lval);
}
