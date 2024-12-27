#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/parse.h"

/*
 * TODO:  
 *      - extract command name
 *      - arguments
 *      - input redirection ( < )
 *      - output redirection ( > )
 *      - piping ( | )
 *      - background execution ( & )
 *
 *  NOTE: 
 *  Understand:
 *      - Order of execution may influence parsing
 *      - how do we parse the arguments
 *      - what delimits arguments
 */


// TODO: Set errno values on error

char **parse(char *input)
{
    // counts number of token the input has i.e the number of words
    // every time a ' ' is detected the counter goes +1
    size_t num_tokens = 0;

    for (int i = 0; i < strlen(input); i++) 
    {
        if (isblank(input[i]))
        {
            num_tokens++;
        }
    }

    // pointer to a pointer -- represents an array of strings 
    char **args = malloc(sizeof(char *) * num_tokens);
    if (args == NULL) {
        perror("malloc(): **args"); 
        exit(EXIT_FAILURE);
    }

    int i = 0;


    // split the input string everytime there is a space 
    char *token = strtok(input, " ");
    while (token != NULL)
    {
        // increments post storing the token is args
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    // NOTE: args should be freed outside of this function
    return args;
}

