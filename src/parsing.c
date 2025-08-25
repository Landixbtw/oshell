#include "../include/Header.h"

// gives you the length of an array
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

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

/*
 * TODO: 
 * starting at first for loop we get error, conditional jump or move depens on uninitialised value(s)
 * why?
 * */

// start_arg is the whole argument [i], the pos is the position of the char [i][j]
char *build_quote_string(char **arg, int start_arg, int start_pos, int end_arg, int end_pos, char quote) {
    int arg_count = 0;
    while(arg && arg[arg_count]) arg_count++;
    char *new_args = malloc((arg_count + 1) * sizeof(char*));
    int index = 0;
    for(int i = start_arg; i < end_arg; i++) {
        for(int j = start_pos+1; j < end_pos; j++) {
            if(arg[i][j] != quote) {
                // we want to keep adding until i++ is a single/double quote
                new_args[index++] = arg[i][j]; 
            }
        }
    }
    new_args[index] = '\0';
    return new_args;
}


// NOTE: Reason I am passing the array, and not just one string at a time is because I am saving the quote, 
// and doing this with passing a string would mean I have to safe the quote char globally no?
// this way it can be saved in the function, because we execute the function once
char **remove_quotes(char **arg) {
    // since we are passing an array, we need to loop through words and letters,
    char quote;
    size_t arg_count = 0;
    while(arg && arg[arg_count]) arg_count++;
    char **new_args = malloc((arg_count + 1) * sizeof(char*));

    bool in_quote = false;
    for(int i = 0; arg[i] != NULL; i++) {
        for(int j = 0; j < strlen(arg[i]); j++) {
            int len = strlen(arg[i]);
            if (len >= 2) {
                // find first quote double or single
                if ((arg[i][j] == '"' || arg[i][j] == '\'') && !in_quote) {
                    int saved_i = i;
                    int saved_j = j;

                    int current_i = i;
                    int current_j = j+1; // keep opening quote in mind

                    int start_arg = i;
                    int start_pos = j;

                    int end_arg = -1;
                    int end_pos = -1;


                    // character is double or single quote, find matching quote
                    quote = arg[i][j];

                    // ---------- claude.ai start

                    while (arg[i][j] != quote) {
                    if (current_j >= strlen(arg[current_i])) {
                        current_i++;  
                        current_j = 0;

                        if (arg[current_i] == NULL) {
                            // Error: no closing quote found
                            break;
                        }
                    }

                    if (arg[current_i][current_j] == quote) {
                        fprintf(stderr, "FOUND CLOSING QUOTE\n");
                        // Found closing quote!
                        end_arg = current_i;
                        end_pos = current_j;
                        break;
                    }
                    current_j++;
                }
                    // ---------- claude.ai end


                    i = saved_i;
                    j = saved_j;

                    if(end_arg >= 0 || end_pos >= 0) {
                        fprintf(stderr, "start arg and pos: %c - %i %i\n", arg[start_arg][start_pos], start_arg, start_pos);
                        fprintf(stderr, "end arg and pos: %c - %i %i\n", arg[end_arg][end_pos], end_arg, end_pos);

                        new_args[i] = build_quote_string(arg, start_arg, start_pos, end_arg, end_pos, quote);
                        memmove(&new_args[i][j], &new_args[i][j+1], len - j);
                        new_args[i][len - 1] = '\0';
                    }

                    // we only want to enter this statement once, after we found one quote, we want to save it and never
                    // enter again
                }
                if(!in_quote) {
                    new_args[i] = malloc(strlen(arg[i]) + 1);
                    strcpy(new_args[i], arg[i]);
                }
            }
        }
    }
    return new_args;
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

    // remove_quotes returns something malloced
    args = remove_quotes(args);


    // NOTE: args should be freed outside of this function
    return args;
}
