#include "../include/Header.h"
#include "../include/parsing_utils.h"

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

    // remove_quotes returns something malloced
    args = remove_quotes(args);


    // NOTE: args should be freed outside of this function
    return args;
}
