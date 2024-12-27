#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/utils.h"
#include "../include/parse.h"

int main(void){
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

        // FIX: Why is > printed if there are multiple lines of output ?, how can this be limited to one

        fprintf(stdout, "> ");

        // FIX: More error handling ie 
        // buffer overflow
        // is the while loop for fgets needed ? ie will there ever be more then one \n in a command

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
                return 1;
            }
            input = new_input;

            // append buffer to input
            // strcat(input, buffer);

            // if the buffer ends with a newline we break
            if (buffer[len - 1] == '\n') { 
                break;
            }
        }

        // is_valid_command(input);
        char **args = parse(input);
        execute_command(args);

        // free(args);
        free(input);
    }
    return 0;
}

