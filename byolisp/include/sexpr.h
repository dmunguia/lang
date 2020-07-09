#ifndef SEXPR_H
#define SEXPR_H

#include "mpc.h"
#include "lenv.h"

struct lval_s;
typedef struct lval_s lval_t;

struct lenv_s;
typedef struct lenv_s lenv_t;

typedef lval_t* (*lbuiltin)(lenv_t*, lval_t*);

typedef union {
    long integer;
    double floating_point;
    char* symbol;
    char* error_message;
    lbuiltin funptr;
} lval_value_t;

typedef struct {
    int count;
    lval_t** cell;
} lval_cell_t;

struct lval_s {
    int type;
    lval_value_t value;
    lval_cell_t cells;
};

enum {
    LVAL_TYPE_INUM,
    LVAL_TYPE_FNUM,
    LVAL_TYPE_SYM,
    LVAL_TYPE_FUN,
    LVAL_TYPE_SEXPR,
    LVAL_TYPE_QEXPR,
    LVAL_TYPE_ERR
};

enum {
    LVAL_ERR_DIV_ZERO,
    LVAL_ERR_UNKNOWN_OP,
    LVAL_ERR_NUM_TOO_LARGE,
    LVAL_ERR_NEG_EXP,
    LVAL_ERR_NOT_A_FUNCTION,
    LVAL_ERR_MISMATCH_DATATYPE,
    LVAL_ERR_INCORRECT_NUM_ARGS,
    LVAL_ERR_NO_ARGS_PASSED,
    LVAL_ERR_UNBOUND_SYMBOL
};

lval_t* sexpr_lval_inum_new(long num);
lval_t* sexpr_lval_fnum_new(double num);
lval_t* sexpr_lval_sym_new(char *sym);
lval_t* sexpr_lval_funptr_new(lbuiltin funptr);
lval_t* sexpr_lval_sexpr_new(void);
lval_t* sexpr_lval_qexpr_new(void);
lval_t* sexpr_lval_err_new(char* fmt, ...);
void sexpr_lval_free(lval_t *val);
lval_t* sexpr_lval_push(lval_t *lval, lval_t *new);
lval_t* sexpr_lval_pop(lval_t *lval, int i);
lval_t* sexpr_lval_take(lval_t *lval, int i);
lval_t* sexpr_lval_join(lval_t *a, lval_t *b);
lval_t* sexpr_lval_copy(lval_t *lval);
lval_t* sexpr_build_from_ast(mpc_ast_t *ast);
char* sexpr_type_to_string(int type);

#endif // SEXPR_H