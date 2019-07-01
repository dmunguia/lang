#include <stdlib.h>
#include <string.h>

#include "builtins.h"
#include "evaluator.h"

static lval_t evaluator_evaluate_number(mpc_ast_t *ast) 
{
    if (strstr(ast->contents, ".")) {
        double number = strtod(ast->contents, NULL);
        return errno != ERANGE ? 
            evaluator_lval_fnum_new(number) : 
            evaluator_lval_err_new(LVAL_ERR_NUM_TOO_LARGE);
    } else {
        errno = 0; // macro
        long number = strtol(ast->contents, NULL, 10);
        return errno != ERANGE ? 
            evaluator_lval_inum_new(number) : 
            evaluator_lval_err_new(LVAL_ERR_NUM_TOO_LARGE);
    }
}

static lval_t evaluator_evaluate_integer_operation(char *operator, long a, long b) 
{

    if (strcmp(operator, "+") == 0) { return evaluator_lval_inum_new(a + b); }
    if (strcmp(operator, "-") == 0) { return evaluator_lval_inum_new(a - b); }
    if (strcmp(operator, "*") == 0) { return evaluator_lval_inum_new(a * b); }
    if (strcmp(operator, "%") == 0) { return evaluator_lval_inum_new(a % b); }
    if (strcmp(operator, "min") == 0) { return evaluator_lval_inum_new(builtin_imin(a, b)); }
    if (strcmp(operator, "max") == 0) { return evaluator_lval_inum_new(builtin_imax(a, b)); }
    if (strcmp(operator, "neg") == 0) { return evaluator_lval_inum_new(-1 * a); }

    if (strcmp(operator, "/") == 0) { 
        return b == 0 ?
            evaluator_lval_err_new(LVAL_ERR_DIV_ZERO) :
            evaluator_lval_inum_new(a / b);
    }

    if (strcmp(operator, "^") == 0) { 
        return b < 0 ?
            evaluator_lval_err_new(LVAL_ERR_NEG_EXP) :
            evaluator_lval_inum_new(builtin_ipow(a, b)); 
    }

    return evaluator_lval_err_new(LVAL_ERR_UNKNOWN_OP);
}

static lval_t evaluator_evaluate_fp_operation(char *operator, double a, double b) 
{

    if (strcmp(operator, "+") == 0) { return evaluator_lval_fnum_new(a + b); }
    if (strcmp(operator, "-") == 0) { return evaluator_lval_fnum_new(a - b); }
    if (strcmp(operator, "*") == 0) { return evaluator_lval_fnum_new(a * b); }
    if (strcmp(operator, "%") == 0) { return evaluator_lval_fnum_new(fmod(a, b)); }
    if (strcmp(operator, "min") == 0) { return evaluator_lval_fnum_new(builtin_fmin(a, b)); }
    if (strcmp(operator, "max") == 0) { return evaluator_lval_fnum_new(builtin_fmax(a, b)); }
    if (strcmp(operator, "neg") == 0) { return evaluator_lval_fnum_new(-1 * a); }

    if (strcmp(operator, "/") == 0) { 
        return b == 0 ?
            evaluator_lval_err_new(LVAL_ERR_DIV_ZERO) :
            evaluator_lval_fnum_new(a / b);
    }

    if (strcmp(operator, "^") == 0) { 
        return b < 0 ?
            evaluator_lval_err_new(LVAL_ERR_NEG_EXP) :
            evaluator_lval_fnum_new(builtin_fpow(a, b)); 
    }

    return evaluator_lval_err_new(LVAL_ERR_UNKNOWN_OP);
}

static lval_t evaluator_evaluate_operator(char *operator, lval_t operand_a, lval_t operand_b)
{
    if (operand_a.type == LVAL_TYPE_ERR) {
        return operand_a;
    }

    if (operand_b.type == LVAL_TYPE_ERR) {
        return operand_b;
    }

    // cast long to double when the expression mixes number types
    if (operand_a.type == LVAL_TYPE_FNUM && operand_b.type == LVAL_TYPE_INUM) {
        long ivalue = operand_b.value.integer_value;
        operand_b.value.double_value = (double) ivalue;
        operand_b.type = LVAL_TYPE_FNUM;

    } else if (operand_a.type == LVAL_TYPE_INUM && operand_b.type == LVAL_TYPE_FNUM) {
        long ivalue = operand_a.value.integer_value;
        operand_a.value.double_value = (double) ivalue;
        operand_a.type = LVAL_TYPE_FNUM;
    }

    if (operand_a.type == LVAL_TYPE_INUM) {
        long a = operand_a.value.integer_value;
        long b = operand_b.value.integer_value;

        return evaluator_evaluate_integer_operation(operator, a, b);

    } else {
        double a = operand_a.value.double_value;
        double b = operand_b.value.double_value;

        return evaluator_evaluate_fp_operation(operator, a, b);
    }

}

lval_t evaluator_lval_inum_new(long num) 
{
    lval_t val;
    val.type = LVAL_TYPE_INUM;
    val.value.integer_value = num;

    return val;
}

lval_t evaluator_lval_fnum_new(double num) 
{
    lval_t val;
    val.type = LVAL_TYPE_FNUM;
    val.value.double_value = num;

    return val;
}

lval_t evaluator_lval_err_new(int err_code) 
{
    lval_t val;
    val.type = LVAL_TYPE_ERR;
    val.value.err_code = err_code;

    return val;
}

lval_t evaluator_evaluate(mpc_ast_t *ast)
{
    if (strstr(ast->tag, "number")) {
        return evaluator_evaluate_number(ast);
    }

    char *operator = ast->children[1]->contents;
    if ((strcmp(operator, "-") == 0) && (ast->children_num == 4)) {
        lval_t partial = evaluator_evaluate(ast->children[2]);
        partial = evaluator_evaluate_operator("neg", partial, evaluator_lval_inum_new(0));

        return partial;

    } else {
        lval_t partial = evaluator_evaluate(ast->children[2]);

        for (int i = 3; strstr(ast->children[i]->tag, "expr"); i++) {
            lval_t current = evaluator_evaluate(ast->children[i]);
            partial = evaluator_evaluate_operator(operator, partial, current);
        }

        return partial;
    }
}

