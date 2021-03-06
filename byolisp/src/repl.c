#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "evaluator.h"
#include "parser.h"
#include "sexpr.h"
#include "lenv.h"

static void repl_lval_print(lval_t* lval);

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
        case LVAL_TYPE_INUM: printf("%li", lval->value.integer); break;
        case LVAL_TYPE_FNUM: printf("%f", lval->value.floating_point); break;
        case LVAL_TYPE_FUN: printf("<function %p>", lval->value.funptr); break;
        case LVAL_TYPE_LAMBDA: 
            printf("(\\ "); 
            repl_lval_print(lval->value.lambda->args); 
            putchar(' '); 
            repl_lval_print(lval->value.lambda->expr); 
            putchar(')'); 
            break;
        case LVAL_TYPE_SYM: printf("%s", lval->value.symbol); break;
        case LVAL_TYPE_SEXPR: repl_lval_sexpr_print(lval, '(', ')'); break;
        case LVAL_TYPE_QEXPR: repl_lval_sexpr_print(lval, '{', '}'); break;
        case LVAL_TYPE_ERR: printf("%s", lval->value.error_message); break;
    }
}

static void repl_lenv_print(lenv_t* lenv)
{
    for (int i = 0; i < lenv->count; i++) {
        char* symbol = lenv->symbols[i];
        lval_t* lval = lenv->values[i];

        switch (lval->type) {
            case LVAL_TYPE_INUM: printf("%s: %li\n", symbol, lval->value.integer); break;
            case LVAL_TYPE_FNUM: printf("%s: %f\n", symbol, lval->value.floating_point); break;
            case LVAL_TYPE_FUN: printf("%s: <function %p>\n", symbol, lval->value.funptr); break;
            case LVAL_TYPE_SYM: printf("%s: %s\n", symbol, lval->value.symbol); break;
            case LVAL_TYPE_SEXPR: printf("%s: ", symbol); repl_lval_sexpr_print(lval, '(', ')'); putchar('\n'); break;
            case LVAL_TYPE_QEXPR: printf("%s: ", symbol); repl_lval_sexpr_print(lval, '{', '}'); putchar('\n'); break;
            case LVAL_TYPE_ERR: printf("%s: %s\n", symbol, lval->value.error_message); break;
        }
    }
}

static void repl_lval_println(lval_t *lval)
{
    repl_lval_print(lval);
    putchar('\n');
}

int main(int argc, char** argv) 
{
    lenv_t* lenv = lenv_new();
    lenv_add_builtins(lenv);

    puts("byolisp 0.0.1");
    puts("ctrl+D to exit\n");

    parser_grammar_t grammar = parser_build_grammar();
    
    char *input;
    while ((input = readline("byolisp> ")) != NULL) {
        add_history(input);

        if (strcmp(input, "env") == 0) {
            repl_lenv_print(lenv);
            free(input);
        } else if (strcmp(input, "exit") == 0) {
            free(input);
            break;
        } else {
            parser_result_t result = parser_parse(input, grammar);
            lval_t *lval = sexpr_build_from_ast(result.result_data.output);
            lval = evaluator_evaluate(lenv, lval);
            repl_lval_println(lval);
            sexpr_lval_free(lval);

            free(input);
        }
    }

    puts("\nexit\n");

    parser_free_grammar(grammar);

    return 0;
}