#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#include "../include/utils.h"
#include "../include/parse.h"


int main(void){
    // show_usage();
    for(;;) {
        // everything happens in here
        // if something happens a child process will be created with fork()
        //
        // for the MVP
        //
        // get command 
        // parse it
        // execute it

        size_t size = 0;
        ssize_t chars_read = 0;
        char *input = NULL;

        printf("> ");
        // TODO: Maybe change input method or   
        if ((chars_read = getline(&input, &size, stdin)) == EOF)
        {
            perror("getline() failed:");
            exit(EXIT_FAILURE);
        }

        if (chars_read < 0)
        {
            printf("getline() failed. status:");
            free(input);
            return 1;
        }


        const char *EXIT_OSHELL_KEYWORD = "exit\n";
        char *command = strtok(input, " ");
        if (is_valid_command(command)) {
            // if user input is exit EXIT
            if (strncmp(input, EXIT_OSHELL_KEYWORD, strlen(input)) == 0)
            {
                printf("exiting oshell ...\n");
                exit(EXIT_SUCCESS);
            }
        }
    
        char **args = parse(input);
        // execute_command(input, args);

        free(input);
    }
    return 0;
}

