#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "evaluator.h"
#include "sexpr.h"

static lval_t* evaluator_evaluate_integer_operation(char *operator, long a, long b) 
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
            sexpr_lval_err_new(LVAL_ERR_DIV_ZERO) :
            sexpr_lval_inum_new(a / b);
    }

    if (strcmp(operator, "^") == 0) { 
        return b < 0 ?
            sexpr_lval_err_new(LVAL_ERR_NEG_EXP) :
            sexpr_lval_inum_new(builtin_ipow(a, b)); 
    }

    return sexpr_lval_err_new(LVAL_ERR_UNKNOWN_OP);
}

static lval_t* evaluator_evaluate_fp_operation(char *operator, double a, double b) 
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
            sexpr_lval_err_new(LVAL_ERR_DIV_ZERO) :
            sexpr_lval_fnum_new(a / b);
    }

    if (strcmp(operator, "^") == 0) { 
        return b < 0 ?
            sexpr_lval_err_new(LVAL_ERR_NEG_EXP) :
            sexpr_lval_fnum_new(builtin_fpow(a, b)); 
    }

    return sexpr_lval_err_new(LVAL_ERR_UNKNOWN_OP);
}

static lval_t* evaluator_evaluate_operator(char *operator, lval_t *operands)
{
    for (int i = 0; i < operands->cells.count; i++) { // all data types must be numeric
        if (operands->cells.cell[i]->type != LVAL_TYPE_INUM && operands->cells.cell[i]->type != LVAL_TYPE_FNUM) {
            sexpr_lval_free(operands);
            return sexpr_lval_err_new(LVAL_ERR_MISMATCH_DATATYPE);
        }
    }

    lval_t *operand_a = sexpr_lval_pop(operands, 0);

    if ((strcmp(operator, "-") == 0) && operands->cells.count == 0) {
        if (operand_a->type == LVAL_TYPE_INUM) {
            operand_a->value.integer_value = -(operand_a->value.integer_value);
        } else if (operand_a->type == LVAL_TYPE_FNUM) {
            operand_a->value.double_value = -(operand_a->value.double_value);
        } else { // this should de impossible due to previous type check
            sexpr_lval_free(operand_a);
            sexpr_lval_free(operands);
            return sexpr_lval_err_new(LVAL_ERR_UNKNOWN_OP);
        }
    }

    while (operands->cells.count > 0) {
        lval_t *operand_b = sexpr_lval_pop(operands, 0);

        // cast long to double when the expression mixes number types
        if (operand_a->type == LVAL_TYPE_FNUM && operand_b->type == LVAL_TYPE_INUM) {
            long ivalue = operand_b->value.integer_value;
            operand_b->value.double_value = (double) ivalue;
            operand_b->type = LVAL_TYPE_FNUM;

        } else if (operand_a->type == LVAL_TYPE_INUM && operand_b->type == LVAL_TYPE_FNUM) {
            long ivalue = operand_a->value.integer_value;
            operand_a->value.double_value = (double) ivalue;
            operand_a->type = LVAL_TYPE_FNUM;
        }

        if (operand_a->type == LVAL_TYPE_INUM) {
            long a = operand_a->value.integer_value;
            long b = operand_b->value.integer_value;

            lval_t *result = evaluator_evaluate_integer_operation(operator, a, b);
            if (result->type == LVAL_TYPE_ERR) {
                sexpr_lval_free(operand_a);
                sexpr_lval_free(operand_b);
                sexpr_lval_free(operands);
                return result;
            } else {
                operand_a->value.integer_value = result->value.integer_value;
                sexpr_lval_free(result);
            }

        } else {
            double a = operand_a->value.double_value;
            double b = operand_b->value.double_value;

            lval_t *result = evaluator_evaluate_fp_operation(operator, a, b);
            if (result->type == LVAL_TYPE_ERR) {
                sexpr_lval_free(operand_a);
                sexpr_lval_free(operand_b);
                sexpr_lval_free(operands);
                return result;
            } else {
                operand_a->value.double_value = result->value.double_value;
                sexpr_lval_free(result);
            }
        }

        sexpr_lval_free(operand_b);
    }

    sexpr_lval_free(operands);
    return operand_a;
}

static lval_t* evaluator_evaluate_sexpr(lval_t *lval)
{
    for (int i = 0; i < lval->cells.count; i++) {
        lval->cells.cell[i] = evaluator_evaluate(lval->cells.cell[i]);
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

    lval_t *symbol = sexpr_lval_pop(lval, 0);
    if (symbol->type != LVAL_TYPE_SYM) {
        sexpr_lval_free(symbol);
        sexpr_lval_free(lval);
        return sexpr_lval_err_new(LVAL_ERR_SEXPR_MUST_START_WITH_SYMBOL);
    }

    lval_t *result = evaluator_evaluate_operator(symbol->value.symbol_value, lval);
    sexpr_lval_free(symbol);

    return result;
}

lval_t* evaluator_evaluate(lval_t *lval)
{
    if (lval->type == LVAL_TYPE_SEXPR) {
        return evaluator_evaluate_sexpr(lval);
    }

    return lval;
}

