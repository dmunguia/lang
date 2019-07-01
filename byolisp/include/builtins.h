#ifndef BUILTINS_H
#define BUILTINS_H

long builtin_imin(long operand_a, long operand_b);
long builtin_imax(long operand_a, long operand_b);
long builtin_ipow(long base, long exp);

double builtin_fmin(double operand_a, double operand_b);
double builtin_fmax(double operand_a, double operand_b);
double builtin_fpow(double base, double exp) ;

#endif // BUILTINS_H