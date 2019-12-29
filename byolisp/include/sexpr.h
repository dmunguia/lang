#ifndef SEXPR_H
#define SEXPR_H

#include "mpc.h"

typedef union {
    long integer_value;
    double double_value;
    char* symbol_value;
} lval_value_t;

struct lval_s;

typedef struct {
    int count;
    struct lval_s** cell;
} lval_cell_t;

typedef struct lval_s {
    int type;
    int err_code;
    lval_value_t value;
    lval_cell_t cells;
} lval_t;

enum {
    LVAL_TYPE_INUM,
    LVAL_TYPE_FNUM,
    LVAL_TYPE_SYM,
    LVAL_TYPE_SEXPR,
    LVAL_TYPE_ERR
};

enum {
    LVAL_ERR_DIV_ZERO,
    LVAL_ERR_UNKNOWN_OP,
    LVAL_ERR_NUM_TOO_LARGE,
    LVAL_ERR_NEG_EXP,
    LVAL_ERR_SEXPR_MUST_START_WITH_SYMBOL,
    LVAL_ERR_MISMATCH_DATATYPE
};

lval_t* sexpr_lval_inum_new(long num);
lval_t* sexpr_lval_fnum_new(double num);
lval_t* sexpr_lval_sym_new(char *sym);
lval_t* sexpr_lval_sexpr_new(void);
lval_t* sexpr_lval_err_new(int err_code);
void sexpr_lval_free(lval_t *val);
lval_t* sexpr_lval_pop(lval_t *lval, int i);
lval_t* sexpr_lval_take(lval_t *lval, int i);
lval_t* sexpr_build_from_ast(mpc_ast_t *ast);

#endif // SEXPR_H