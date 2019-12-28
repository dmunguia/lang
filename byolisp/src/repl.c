#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "parser.h"
#include "sexpr.h"

static void repl_lval_print(lval_t* lval);

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

static void repl_lval_sexpr_print(lval_t *lval, char open, char close)
{
    putchar(open);
    for (int i = 0; i < lval->cells.count; i++) {
        repl_lval_print(lval->cells.cell[i]);

        if (i < (lval->cells.count - 1)) {
            putchar(' ');
        }
    }
    putchar(close);
}

static void repl_lval_print(lval_t *lval) 
{
    switch (lval->type) {
        case LVAL_TYPE_INUM: printf("%li", lval->value.integer_value); break;
        case LVAL_TYPE_FNUM: printf("%f", lval->value.double_value); break;
        case LVAL_TYPE_SYM: printf("%s", lval->value.symbol_value); break;
        case LVAL_TYPE_SEXPR: repl_lval_sexpr_print(lval, '(', ')'); break;
        case LVAL_TYPE_ERR: repl_lval_report_error(lval->err_code); break;
    }
}

static void repl_lval_println(lval_t *lval)
{
    repl_lval_print(lval);
    putchar('\n');
}

int main(int argc, char** argv) 
{
    puts("byolisp 0.0.1");
    puts("ctrl+D to exit\n");

    parser_grammar_t grammar = parser_build_grammar();
    
    char *input;
    while ((input = readline("byolisp> ")) != NULL) {
        add_history(input);

        parser_result_t result = parser_parse(input, grammar);
        //parser_report_output(result);
        //lval_t value = evaluator_evaluate(result.result_data.output);
        //repl_lval_print(value);

        lval_t *lval = sexpr_build_from_ast(result.result_data.output);
        repl_lval_println(lval);
        sexpr_lval_free(lval);

        free(input);
    }

    puts("\nexit\n");

    parser_free_grammar(grammar);

    return 0;
}