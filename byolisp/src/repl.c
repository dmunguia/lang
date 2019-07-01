#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "parser.h"
#include "evaluator.h"

static void repl_lval_report_error(int err) 
{
    if (err == LVAL_ERR_DIV_ZERO) {
        printf("Error: division by zero");
    } else if (err == LVAL_ERR_UNKNOWN_OP) {
        printf("Error: unknown operator");
    } else if (err == LVAL_ERR_NUM_TOO_LARGE) {
        printf("Error: number too large");
    } else if (err == LVAL_ERR_NEG_EXP) {
        printf("Error: exponent can't be negative");
    } else {
        printf("Error: unknown error code %i", err);
    }
}

static void repl_lval_print(lval_t lval) 
{
    switch (lval.type) {
        case LVAL_TYPE_INUM: printf("%li", lval.value.integer_value); break;
        case LVAL_TYPE_FNUM: printf("%f", lval.value.double_value); break;
        case LVAL_TYPE_ERR: repl_lval_report_error(lval.value.err_code); break;
    }

    putchar('\n');
}

int main(int argc, char** argv) 
{
    puts("byolisp 0.0.1");
    puts("ctrl+D to exit\n");

    parser_grammar_t *grammar = parser_build_grammar();
    
    char *input;
    while ((input = readline("byolisp> ")) != NULL) {
        add_history(input);

        parser_result_t *result = parser_parse(input, grammar);
        //parser_report_output(result);
        lval_t value = evaluator_evaluate(result->result_data->output);
        repl_lval_print(value);

        parser_free_result(result);

        free(input);
    }

    puts("\nexit\n");

    parser_free_grammar(grammar);

    return 0;
}