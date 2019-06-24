#include <stdlib.h>

#include "mpc.h"
#include "parser.h"

parser_grammar_t* parser_build_grammar()
{
    parser_grammar_t *grammar = malloc(sizeof(parser_grammar_t));

    mpc_parser_t *Number = mpc_new("number");
    mpc_parser_t *Operator = mpc_new("operator");
    mpc_parser_t *Expr = mpc_new("expr");
    mpc_parser_t *BYOLisp = mpc_new("byolisp");

    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                       \
            number   : /-?[0-9]+/ ;                             \
            operator : '+' | '-' | '*' | '/' ;                  \
            expr     : <number> | '(' <operator> <expr>+ ')' ;  \
            byolisp  : /^/ <operator> <expr>+ /$/ ;             \
        ", 
        Number, Operator, Expr, BYOLisp, NULL);

    grammar->Number = Number;
    grammar->Operator = Operator;
    grammar->Expr = Expr;
    grammar->BYOLisp = BYOLisp;

    return grammar;
}

parser_result_t* parser_parse(const char *input, parser_grammar_t *grammar) 
{
    parser_result_t *result = malloc(sizeof(parser_result_t));
    result->result_data = malloc(sizeof(mpc_result_t));

    int flag = mpc_parse("<stdin>", input, grammar->BYOLisp, result->result_data);
    result->result_flag = flag;

    return result;
}

void parser_free_grammar(parser_grammar_t *grammar)
{
    mpc_cleanup(4, grammar->Number, grammar->Operator, grammar->Expr, grammar->BYOLisp);
    free(grammar);
}

void parser_report_output(parser_result_t *result) 
{
    if (result->result_flag != 0) {
        mpc_ast_print(result->result_data->output);
        mpc_ast_delete(result->result_data->output);
    } else {
        mpc_err_print(result->result_data->error);
        mpc_err_delete(result->result_data->error);
    }
}

void parser_free_result(parser_result_t *result)
{
    free(result);
}