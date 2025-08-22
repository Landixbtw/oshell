#include "../include/Header.h"

#define KEY_DEL 127

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


// NOTE: Reason I am passing the array, and not just one string at a time is because I am saving the quote, 
// and doing this with passing a string would mean I have to safe the quote char globally no?
// this way it can be saved in the function, because we execute the function once
char **remove_quotes(char **arg) {
    // since we are passing an array, we need to loop through words and letters,
    int pos;
    char quote;
    char **new_args = malloc(ARRAY_SIZE(arg));
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

                    // we want to keep adding until i++ is a single/double quote
                    new_args[i] = arg[i];
                    in_quote = true;
                    // we only want to enter this statement once, after we found one quote, we want to save it and never
                    // enter again
                }
                if(!in_quote) {
                    new_args[i] = arg[i];
                }

                // then we want to compare every char until we find a matching one
                if (arg[i][j] == quote) {
                    // we found the matching quote, we want to memmove x to the left, and set arg[i] to '\0' or ""?
                    memmove(&arg[i][j], &arg[i][j+1], len - j);
                    arg[i][len - 1] = '\0';
                }
            }
        }
    }
    return arg;
}

// this function captures raw key input, for most promenantly DEL
// returns -1 on no key captured, or error
int read_key(void) {
    char ch = -1;
    
    if(read(STDIN_FILENO, &ch, 1) != 1) return -1;

    if(ch == KEY_DEL) {
        return KEY_DEL;
    } else if (ch == 27) {
        // ...
        // return some other key or something
    }
    return ch;
}




// TODO: UNDERSTAND THIS
char **tokenize(char *input, int capacity) {
    char **tokens = malloc(sizeof(char *) * capacity);
    if (!tokens) return NULL;

    int t = 0;
    int start = 0;
    int len = strlen(input);

    for (int i = 0; i <= len; i++) { 
        if (input[i] == ' ' || input[i] == '\0') {
            if (i > start) {
                input[i] = '\0';
                tokens[t++] = &input[start]; 
                if (t >= capacity - 1) { 
                    capacity *= 2;
                    tokens = realloc(tokens, sizeof(char *) * capacity);
                }
            }
            start = i + 1; // next token starts after space
        }
    }
    tokens[t] = NULL; // null-terminate the array
    return tokens;
}



// return has to be freed
char **parse(char *input)
{

    // first we check for the DEL KEY press, if that is true, we dont need parse anything
    // char **KEY_DEL_EVENT;
    // if(read_key() == KEY_DEL) {
    //     KEY_DEL_EVENT[0] = "127";
    // }

    int capacity = 10;
    int count = 0;
    char **args = malloc(sizeof(char *) * capacity);

    // pointer to a pointer -- represents an array of strings 
    // args should never be NULL
    if(args == NULL) {
        perror("oshell: CRITICAL ERROR **args is NULL");
        exit(EXIT_FAILURE);
    }

    if (count >= capacity - 1) {  
        capacity *= 2;
        args = realloc(args, sizeof(char *) * capacity);
        if(args == NULL) {
            perror("oshell: parsing() realloc failed ");
        }
    }

    int i = 0;

    args = tokenize(input, capacity);

    while(args[i] != NULL) // SUMMARY: AddressSanitizer: heap-buffer-overflow ../src/parsing.c:HERE in parse
    {
        i++;
    }

    // go one back after the NULL to be at the last char
    if(i > 0) i--;
    else {
        perror("?");
    }

    if(args[i] == NULL) { exit(-1);}
    char *mod_str = args[i];

    if(mod_str == NULL) perror("oshell: parsing()");
    // replace \n with \0
    if (strlen(mod_str) > 0 && mod_str[strlen(mod_str) - 1] == '\n') {
        mod_str[strlen(mod_str) - 1] = '\0';
    }

    /*
     * We want to remove the quotes after tokeninazation so that we dont mess up the tokens
     * ["test 1 2 3"] is not the same as [test] [1] [2] [3]
     * */

    for(int i = 0; args[i] != NULL; i++) 
        fprintf(stderr, "args[%i]: %s\n", i,args[i]);

    args = remove_quotes(args);

    for(int i = 0; args[i] != NULL; i++) 
        fprintf(stderr, "args[%i]: %s\n", i,args[i]);

    // NOTE: args should be freed outside of this function
    return args;
}
