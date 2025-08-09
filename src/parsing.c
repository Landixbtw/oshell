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

// claude.ai made v1, only removing quotes from one word not multiple args ie "foo bar baz"
//
/* 
 * Recognize that the opening quote starts a quoted string 
 * Continue reading characters (including spaces) until you find the matching closing quote 
 * Treat everything inside as a single token 
 * Handle escaped quotes within quoted strings
 *
 * everything in between quotes will be treated as a single argument
 * */

// can I still use this? or do I need a whole new function
// void remove_quotes(char *arg) {
//     int len = strlen(arg);
//     if (len >= 2) {
//         if ((arg[0] == '"' && arg[len-1] == '"') ||
//             (arg[0] == '\'' && arg[len-1] == '\'')) {
//             // Shift everything left
//             // with memmove we move everything to the left, the left quote "falls out" of the string, then we set the null terminator to where the
//             // last quote was, and boom we removed both
//             memmove(arg, arg+1, len-2);
//             arg[len-2] = '\0';
//         }
//     }
// }

// NOTE: Reason I am passing the array, and not just one string at a time is because I am saving the quote, 
// and doing this with passing a string would mean I have to safe the quote char globally no?
// this way it can be saved in the function, because we execute the function once
char **remove_quotes(char **arg) {
    // since we are passing an array, we need to loop through words and letters,
    int pos;
    char quote;
    bool in_quote = false;
    for(int i = 0; arg[i] != NULL; i++) {
        for(int j = 0; j < strlen(arg[i]); j++) {
            int len = strlen(arg[i]);
            if (len >= 2) {
                // find first quote double or single
                if ((arg[i][j] == '"' || arg[i][j] == '\'') && !in_quote) {
                    // character is double or single quote, find matching quote
                    quote = *arg[i];
                    pos = j;
                    in_quote = true;
                    // we only want to enter this statement once, after we found one quote, we want to save it and never
                    // enter again
                }
                // then we want to compare every char until we find a matching one
                if (arg[i][j] == quote) {
                    // we found the matching quote, we want to memmove x to the left, and set arg[i] to '\0' or ""?
                    memmove(&arg[i][j], &arg[i][j+1], len - j);
                }
            }
        }
    }
    return arg;
}

char **parse(char *input)
{
    // counts number of token the input has i.e the number of words
    // every time a ' ' is detected the counter goes +1
    size_t num_tokens = 0;

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


// the "" are not removed 
// echo "foo bar baz" | wc -w

    // split the input string everytime there is a space 
    char *token = strtok(input, " ");

    int i = 0;
    do {
        // increments post storing the token is args
        args[i] = token;
        i++;
        token = strtok(NULL, " ");
    }while (token != NULL);


    while(args[i] != NULL) 
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


    args = remove_quotes(args);
   // for(int j = 0; args[j] != NULL; j++) {
   //     fprintf(stderr,"args[%i]: %s \n", j, args[j]);
   //  }

    // NOTE: args should be freed outside of this function
    return args;
}
