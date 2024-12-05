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


        char *input = malloc(1025);
        char buffer[sizeof(input)] = "\n";

        if(!input) {
            perror("malloc() failed");
            free(input);
        }

        // input[0] = '\0';

        printf("> ");



        while(fgets(input, sizeof(input), stdin))
        {
            /* Checks if the last char in buffer is newline */
            size_t len = strlen(buffer);

            // resize memory for input 
            // we need to add one for null terminator
            size_t new_size = strlen(input) + len + 1;
            char *new_input = realloc(input, new_size);

            if(!new_input) {
                perror("realloc() failed");
                free(input);
                free(new_input);
                return 1;
            }

            input = new_input;

            // append buffer to input
            strcat(input, buffer);

            // if the buffer ends with a newline we break
            if (buffer[len - 1] == '\n') free(new_input); break;
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

        // free(args);
        free(input);
    }
    return 0;
}

