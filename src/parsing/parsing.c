#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
    char **args = malloc(sizeof(char *) * num_tokens); // WARN: ADDRESS IS 0 BYTES AFTER A BLOCK OF SIZE 8 was ALLOCD
    // args should never be NULL
    assert(args != NULL);

    int i = 0;

    #define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

    // split the input string everytime there is a space 
    char *token = strtok(input, " ");
    do {
        printf("token: %s ",token);
        // increments post storing the token is args
        args[i++] = token;
        printf("args length: %ld\n", NELEMS(args));

        token = strtok(NULL, " ");
    }while (token != NULL);

    // go till char is NULL
    while(args[i] != NULL)
    {
        i++;
    }

    // go one back after the NULL to be at the last char
    i--;

    char *mod_str = args[i];
    size_t len = strlen(mod_str);
    // this should never be NULL
    assert(mod_str != NULL);

    // replace \n with \0
    if (len > 0 && mod_str[len - 1] == '\n') {
        mod_str[len - 1] = '\0';
    }

    // NOTE: args should be freed outside of this function
    return args;
}

