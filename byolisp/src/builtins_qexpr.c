#include "sexpr.h"
#include "evaluator.h"
#include "builtins_qexpr.h"


lval_t* builtin_qexpr_head(lenv_t* lenv, lval_t *lval) 
{
    QEXPR_ASSERT(lval, lval->cells.count == 1, 
        "Function 'head' passed incorrect number of arguments. Received %i, but was expecting 1",
        lval->cells.count);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->type == LVAL_TYPE_QEXPR, 
        "Function 'head' passed invalid argument data type. Received %s, but was expecting %s",
        sexpr_type_to_string(lval->cells.cell[0]->type), sexpr_type_to_string(LVAL_TYPE_QEXPR));
    QEXPR_ASSERT(lval, lval->cells.cell[0]->cells.count > 0, "Function 'head' passed empty list");

    lval_t *head = sexpr_lval_take(lval, 0);
    while (head->cells.count > 1) {
        sexpr_lval_free(sexpr_lval_pop(head, 1));
    }
    
    return head;
}

lval_t* builtin_qexpr_tail(lenv_t* lenv, lval_t *lval) 
{
    QEXPR_ASSERT(lval, lval->cells.count == 1, 
        "Function 'tail' passed incorrect number of arguments. Received %i, but was expecting 1",
        lval->cells.count);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->type == LVAL_TYPE_QEXPR, 
        "Function 'tail' passed invalid argument data type. Received %s, but was expecting %s",
        sexpr_type_to_string(lval->cells.cell[0]->type), sexpr_type_to_string(LVAL_TYPE_QEXPR));
    QEXPR_ASSERT(lval, lval->cells.cell[0]->cells.count > 0, "Function 'tail' passed empty list");

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
    QEXPR_ASSERT(lval, lval->cells.count == 1, 
        "Function 'eval' passed incorrect number of arguments. Received %i, but was expecting 1",
        lval->cells.count);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->type == LVAL_TYPE_QEXPR, 
        "Function 'eval' passed invalid argument data type. Received %s, but was expecting %s",
        sexpr_type_to_string(lval->cells.cell[0]->type), sexpr_type_to_string(LVAL_TYPE_QEXPR));

    lval_t *sexpr = sexpr_lval_take(lval, 0);
    sexpr->type = LVAL_TYPE_SEXPR;
    return evaluator_evaluate(lenv, sexpr);
}

lval_t* builtin_qexpr_join(lenv_t* lenv, lval_t *lval) 
{
    for (int i = 0; i < lval->cells.count; i++) {
        QEXPR_ASSERT(lval, lval->cells.cell[i]->type == LVAL_TYPE_QEXPR, 
            "Function 'join' passed invalid argument data type. Received %s, but was expecting %s",
            sexpr_type_to_string(lval->cells.cell[i]->type), sexpr_type_to_string(LVAL_TYPE_QEXPR));
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
    QEXPR_ASSERT(lval, lval->cells.count == 2, 
        "Function 'cons' passed incorrect number of arguments. Received %i, but was expecting 2",
        lval->cells.count);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->type != LVAL_TYPE_SEXPR, 
        "Function 'cons' passed invalid argument data type. Was expecting a type different to %s as first argument",
        sexpr_type_to_string(LVAL_TYPE_SEXPR));
    QEXPR_ASSERT(lval, lval->cells.cell[1]->type == LVAL_TYPE_QEXPR, 
        "Function 'cons' passed invalid argument data type. Received %s, but was expecting %s as second argument",
        sexpr_type_to_string(lval->cells.cell[1]->type), sexpr_type_to_string(LVAL_TYPE_QEXPR));

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
    QEXPR_ASSERT(lval, lval->cells.count == 1, 
        "Function 'len' passed incorrect number of arguments. Received %i, but was expecting 1",
        lval->cells.count);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->type == LVAL_TYPE_QEXPR, 
        "Function 'len' passed invalid argument data type. Received %s, but was expecting %s",
        sexpr_type_to_string(lval->cells.cell[0]->type), sexpr_type_to_string(LVAL_TYPE_QEXPR));

    lval_t *qexpr = sexpr_lval_pop(lval, 0);
    
    sexpr_lval_free(lval);

    return sexpr_lval_inum_new(qexpr->cells.count);
}

lval_t* builtin_qexpr_init(lenv_t* lenv, lval_t *lval)
{
    QEXPR_ASSERT(lval, lval->cells.count == 1, 
        "Function 'init' passed incorrect number of arguments. Received %i, but was expecting 1",
        lval->cells.count);
    QEXPR_ASSERT(lval, lval->cells.cell[0]->type == LVAL_TYPE_QEXPR, 
        "Function 'init' passed invalid argument data type. Received %s, but was expecting %s",
        sexpr_type_to_string(lval->cells.cell[0]->type), sexpr_type_to_string(LVAL_TYPE_QEXPR));

    lval_t *qexpr = sexpr_lval_pop(lval, 0);
    
    sexpr_lval_free(lval);

    sexpr_lval_pop(qexpr, qexpr->cells.count - 1);

    return qexpr;
}
