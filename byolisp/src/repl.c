#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

int main(int argc, char** argv) 
{
    puts("byolisp 0.0.1");
    puts("ctrl+c to exit\n");
    
    while (1) {
        char* input = readline("byolisp> ");
        add_history(input);

        printf("echo: %s \n", input);

        free(input);
    }

    return 0;
}