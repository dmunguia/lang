#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "evaluator.h"

static long evaluator_builtin_min(long operand_a, long operand_b)
{
    return (operand_a < operand_b) ? operand_a : operand_b;
}

static long evaluator_builtin_max(long operand_a, long operand_b)
{
    return (operand_a > operand_b) ? operand_a : operand_b;
}

static long evaluator_builtin_ipow(long base, long exp)
{
    long result = 1;
    for (;;) {
        if (exp & 1) {
            result *= base;
        }
        exp >>= 1;
        if (!exp) {
            break;
        }
        base *= base;
    }

    return result;
}

static double evaluator_builtin_fpow(double base, double exp) 
{
    return pow(base, exp);
}

static lval_t evaluator_evaluate_number(mpc_ast_t *ast) 
{
    errno = 0; // macro
    long number = strtol(ast->contents, NULL, 10);
    return errno != ERANGE ? 
        evaluator_lval_inum_new(number) : 
        evaluator_lval_err_new(LVAL_ERR_NUM_TOO_LARGE);
}

static lval_t evaluator_evaluate_operator(char *operator, lval_t operand_a, lval_t operand_b)
{
    if (operand_a.type == LVAL_TYPE_ERR) {
        return operand_a;
    }

    if (operand_b.type == LVAL_TYPE_ERR) {
        return operand_b;
    }

    long a = operand_a.value.integer_value;
    long b = operand_b.value.integer_value;

    if (strcmp(operator, "+") == 0) { return evaluator_lval_inum_new(a + b); }
    if (strcmp(operator, "-") == 0) { return evaluator_lval_inum_new(a - b); }
    if (strcmp(operator, "*") == 0) { return evaluator_lval_inum_new(a * b); }
    if (strcmp(operator, "%") == 0) { return evaluator_lval_inum_new(a % b); }
    if (strcmp(operator, "min") == 0) { return evaluator_lval_inum_new(evaluator_builtin_min(a, b)); }
    if (strcmp(operator, "max") == 0) { return evaluator_lval_inum_new(evaluator_builtin_max(a, b)); }
    if (strcmp(operator, "neg") == 0) { return evaluator_lval_inum_new(-1 * a); }

    if (strcmp(operator, "/") == 0) { 
        return b == 0 ?
            evaluator_lval_err_new(LVAL_ERR_DIV_ZERO) :
            evaluator_lval_inum_new(a / b);
    }

    if (strcmp(operator, "^") == 0) { 
        return b < 0 ?
            evaluator_lval_err_new(LVAL_ERR_NEG_EXP) :
            evaluator_lval_inum_new(evaluator_builtin_ipow(a, b)); 
    }

    return evaluator_lval_err_new(LVAL_ERR_UNKNOWN_OP);
}

lval_t evaluator_lval_inum_new(long num) 
{
    lval_t val;
    val.type = LVAL_TYPE_NUM;
    val.value.integer_value = num;

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

