#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "parser.h"
#include "evaluator.h"

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
        long value = evaluator_evaluate(result->result_data->output);
        printf("%li\n", value);
        parser_free_result(result);

        free(input);
    }

    puts("\nexit\n");

    parser_free_grammar(grammar);

    return 0;
}