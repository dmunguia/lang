#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "evaluator.h"

static long evaluator_min(long operand_a, long operand_b)
{
    return (operand_a < operand_b) ? operand_a : operand_b;
}

static long evaluator_max(long operand_a, long operand_b)
{
    return (operand_a > operand_b) ? operand_a : operand_b;
}

static long evaluator_pow(long base, long exp)
{
    long result = 1;
    for (;;) {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}

static long evaluator_evaluate_operator(char *operator, long operand_a, long operand_b)
{
    if (strcmp(operator, "+") == 0) { return operand_a + operand_b; }
    if (strcmp(operator, "-") == 0) { return operand_a - operand_b; }
    if (strcmp(operator, "*") == 0) { return operand_a * operand_b; }
    if (strcmp(operator, "/") == 0) { return operand_a / operand_b; }
    if (strcmp(operator, "^") == 0) { return evaluator_pow(operand_a, operand_b); }
    if (strcmp(operator, "%") == 0) { return operand_a % operand_b; }
    if (strcmp(operator, "min") == 0) { return evaluator_min(operand_a, operand_b); }
    if (strcmp(operator, "max") == 0) { return evaluator_max(operand_a, operand_b); }
    if (strcmp(operator, "neg") == 0) { return -1 * operand_a; }

    return 0;
}

long evaluator_evaluate(mpc_ast_t *ast)
{
    if (strstr(ast->tag, "number")) {
        return atoi(ast->contents);
    }

    char *operator = ast->children[1]->contents;
    if ((strcmp(operator, "-") == 0) && (ast->children_num == 4)) {
        long partial = evaluator_evaluate(ast->children[2]);
        partial = evaluator_evaluate_operator("neg", partial, 0);

        return partial;
    } else {
        long partial = evaluator_evaluate(ast->children[2]);

        for (int i = 3; strstr(ast->children[i]->tag, "expr"); i++) {
            long current = evaluator_evaluate(ast->children[i]);
            partial = evaluator_evaluate_operator(operator, partial, current);
        }

        return partial;
    }

}

