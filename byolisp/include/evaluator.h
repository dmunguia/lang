#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "mpc.h"

typedef union {
    int err_code;
    long integer_value;
} lval_value_t;

typedef struct {
    int type;
    lval_value_t value;
} lval_t;

enum {
    LVAL_TYPE_NUM,
    LVAL_TYPE_ERR
};

enum {
    LVAL_ERR_DIV_ZERO,
    LVAL_ERR_UNKNOWN_OP,
    LVAL_ERR_NUM_TOO_LARGE,
    LVAL_ERR_NEG_EXP
};

lval_t evaluator_lval_inum_new(long num);
lval_t evaluator_lval_fnum_new(double num);
lval_t evaluator_lval_err_new(int err);
lval_t evaluator_evaluate(mpc_ast_t *ast);

#endif // EVALUATOR_H