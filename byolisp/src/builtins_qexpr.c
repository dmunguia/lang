#include "sexpr.h"
#include "evaluator.h"
#include "builtins_qexpr.h"


lval_t* builtin_qexpr_head(lval_t *lval) {
    QEXPR_ASSERT(lval, lval->cells.count == 1, LVAL_ERR_TOO_MANY_ARGS_PASSED);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->type == LVAL_TYPE_QEXPR, LVAL_ERR_MISMATCH_DATATYPE);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->cells.count > 0, LVAL_ERR_NO_ARGS_PASSED);

    lval_t *head = sexpr_lval_take(lval, 0);
    while (head->cells.count > 1) {
        sexpr_lval_free(sexpr_lval_pop(head, 1));
    }
    
    return head;
}

lval_t* builtin_qexpr_tail(lval_t *lval) {
    QEXPR_ASSERT(lval, lval->cells.count == 1, LVAL_ERR_TOO_MANY_ARGS_PASSED);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->type == LVAL_TYPE_QEXPR, LVAL_ERR_MISMATCH_DATATYPE);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->cells.count > 0, LVAL_ERR_NO_ARGS_PASSED);

    lval_t *tail = sexpr_lval_take(lval, 0);
    sexpr_lval_free(sexpr_lval_pop(tail, 0));

    return tail;
}

lval_t* builtin_qexpr_list(lval_t *lval) {
    lval->type = LVAL_TYPE_QEXPR;
    return lval;
}

lval_t* builtin_qexpr_eval(lval_t *lval) {
    QEXPR_ASSERT(lval, lval->cells.count == 1, LVAL_ERR_TOO_MANY_ARGS_PASSED);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->type == LVAL_TYPE_QEXPR, LVAL_ERR_MISMATCH_DATATYPE);

    lval_t *sexpr = sexpr_lval_take(lval, 0);
    sexpr->type = LVAL_TYPE_SEXPR;
    return evaluator_evaluate(sexpr);
}

lval_t* builtin_qexpr_join(lval_t *lval) {
    for (int i = 0; i < lval->cells.count; i++) {
        QEXPR_ASSERT(lval, lval->cells.cell[0]->type == LVAL_TYPE_QEXPR, LVAL_ERR_MISMATCH_DATATYPE);
    }

    lval_t *joined = sexpr_lval_pop(lval, 0);
    while (lval->cells.count) {
        joined = sexpr_lval_join(joined, sexpr_lval_pop(lval, 0));
    }

    sexpr_lval_free(lval);

    return joined;
}
