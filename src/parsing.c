#include "../include/Header.h"

/*
 * TODO:  
 *      - extract command name
 *      - arguments
 *      - input redirection ( < )
 *      - output redirection ( > )
 *      - piping ( | )
 *      - background execution ( & )
 *
 */

// claude.ai
void remove_outer_quotes(char *arg) {
    int len = strlen(arg);
    if (len >= 2) {
        if ((arg[0] == '"' && arg[len-1] == '"') ||
            (arg[0] == '\'' && arg[len-1] == '\'')) {
            // Shift everything left
            memmove(arg, arg+1, len-2);
            arg[len-2] = '\0';
        }
    }
}

char **parse(char *input)
{
    // counts number of token the input has i.e the number of words
    // every time a ' ' is detected the counter goes +1
    size_t num_tokens = 0;

    int i = 0;

    for (int j = 0; j < strlen(input); j++)
    {
        if (isblank(input[j]))
        {
            num_tokens++;
        }
    }

    // pointer to a pointer -- represents an array of strings 
    char **args = malloc(sizeof(char *) * strlen(input) + 1);
    // args should never be NULL
    if(args == NULL) {
        perror("oshell: CRITICAL ERROR **args is NULL");
        exit(EXIT_FAILURE);
    }

    // split the input string everytime there is a space 
    char *token = strtok(input, " ");

    /*
     * We want to remove matching "" '' something like echo 'she said "hello"' should output she said "hello" 
     * echo "Don't" should output Don't, 
     * Dont mix "' 
     * */

    do {
        // increments post storing the token is args
        args[i] = token;
        i++;
        token = strtok(NULL, " ");
    }while (token != NULL);

    // go till char is NULL

    // for(int j = 0; j < i; j++) {
    //    fprintf(stderr,"args[%i]: %s \n", j, args[j]);
    // }

    // NOTE: ==32488== Conditional jump or move depends on uninitialised value(s)
    // ==32488==  Uninitialised value was created by a heap allocation
    while(args[i] != NULL) // FIX: Invalid read size of 8 
    {
        i++;
    }

    // go one back after the NULL to be at the last char
    i--;

    char *mod_str = args[i];
    const size_t len = strlen(mod_str);

    if(mod_str == NULL) perror("oshell: parsing()");
    // replace \n with \0
    if (len > 0 && mod_str[len - 1] == '\n') {
        mod_str[len - 1] = '\0';
    }

    for(int i = 0; args[i] != NULL; i++) {
        remove_outer_quotes(args[i]);
    }

    // NOTE: args should be freed outside of this function
    return args;
}
