#include <math.h>

#include "builtins.h"

long builtin_imin(long operand_a, long operand_b)
{
    return (operand_a < operand_b) ? operand_a : operand_b;
}

long builtin_imax(long operand_a, long operand_b)
{
    return (operand_a > operand_b) ? operand_a : operand_b;
}

long builtin_ipow(long base, long exp)
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

double builtin_fmin(double operand_a, double operand_b)
{
    return (operand_a < operand_b) ? operand_a : operand_b;
}

double builtin_fmax(double operand_a, double operand_b)
{
    return (operand_a > operand_b) ? operand_a : operand_b;
}

double builtin_fpow(double base, double exp) 
{
    return pow(base, exp);
}

