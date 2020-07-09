#include "sexpr.h"
#include "evaluator.h"
#include "builtins_qexpr.h"


lval_t* builtin_qexpr_head(lenv_t* lenv, lval_t *lval) 
{
    QEXPR_ASSERT(lval, lval->cells.count == 1, LVAL_ERR_INCORRECT_NUM_ARGS);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->type == LVAL_TYPE_QEXPR, LVAL_ERR_MISMATCH_DATATYPE);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->cells.count > 0, LVAL_ERR_NO_ARGS_PASSED);

    lval_t *head = sexpr_lval_take(lval, 0);
    while (head->cells.count > 1) {
        sexpr_lval_free(sexpr_lval_pop(head, 1));
    }
    
    return head;
}

lval_t* builtin_qexpr_tail(lenv_t* lenv, lval_t *lval) 
{
    QEXPR_ASSERT(lval, lval->cells.count == 1, LVAL_ERR_INCORRECT_NUM_ARGS);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->type == LVAL_TYPE_QEXPR, LVAL_ERR_MISMATCH_DATATYPE);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->cells.count > 0, LVAL_ERR_NO_ARGS_PASSED);

    lval_t *tail = sexpr_lval_take(lval, 0);
    sexpr_lval_free(sexpr_lval_pop(tail, 0));

    return tail;
}

lval_t* builtin_qexpr_list(lenv_t* lenv, lval_t *lval) 
{
    lval->type = LVAL_TYPE_QEXPR;
    return lval;
}

lval_t* builtin_qexpr_eval(lenv_t* lenv, lval_t *lval) 
{
    QEXPR_ASSERT(lval, lval->cells.count == 1, LVAL_ERR_INCORRECT_NUM_ARGS);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->type == LVAL_TYPE_QEXPR, LVAL_ERR_MISMATCH_DATATYPE);

    lval_t *sexpr = sexpr_lval_take(lval, 0);
    sexpr->type = LVAL_TYPE_SEXPR;
    return evaluator_evaluate(lenv, sexpr);
}

lval_t* builtin_qexpr_join(lenv_t* lenv, lval_t *lval) 
{
    for (int i = 0; i < lval->cells.count; i++) {
        QEXPR_ASSERT(lval, lval->cells.cell[i]->type == LVAL_TYPE_QEXPR, LVAL_ERR_MISMATCH_DATATYPE);
    }

    lval_t *joined = sexpr_lval_pop(lval, 0);
    while (lval->cells.count) {
        joined = sexpr_lval_join(joined, sexpr_lval_pop(lval, 0));
    }

    sexpr_lval_free(lval);

    return joined;
}

lval_t* builtin_qexpr_cons(lenv_t* lenv, lval_t *lval)
{
    QEXPR_ASSERT(lval, lval->cells.count == 2, LVAL_ERR_INCORRECT_NUM_ARGS);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->type != LVAL_TYPE_SEXPR, LVAL_ERR_MISMATCH_DATATYPE);
    QEXPR_ASSERT(lval, lval->cells.cell[1]->type == LVAL_TYPE_QEXPR, LVAL_ERR_MISMATCH_DATATYPE);

    lval_t *new = sexpr_lval_pop(lval, 0);
    lval_t *new_qexpr = sexpr_lval_qexpr_new();
    sexpr_lval_push(new_qexpr, new);

    lval_t *qexpr = sexpr_lval_pop(lval, 0);
    sexpr_lval_join(new_qexpr, qexpr);

    sexpr_lval_free(lval);

    return new_qexpr;
}

lval_t* builtin_qexpr_len(lenv_t* lenv, lval_t *lval)
{
    QEXPR_ASSERT(lval, lval->cells.count == 1, LVAL_ERR_INCORRECT_NUM_ARGS);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->type == LVAL_TYPE_QEXPR, LVAL_ERR_MISMATCH_DATATYPE);

    lval_t *qexpr = sexpr_lval_pop(lval, 0);
    
    sexpr_lval_free(lval);

    return sexpr_lval_inum_new(qexpr->cells.count);
}

lval_t* builtin_qexpr_init(lenv_t* lenv, lval_t *lval)
{
    QEXPR_ASSERT(lval, lval->cells.count == 1, LVAL_ERR_INCORRECT_NUM_ARGS);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->type == LVAL_TYPE_QEXPR, LVAL_ERR_MISMATCH_DATATYPE);

    lval_t *qexpr = sexpr_lval_pop(lval, 0);
    
    sexpr_lval_free(lval);

    sexpr_lval_pop(qexpr, qexpr->cells.count - 1);

    return qexpr;
}
