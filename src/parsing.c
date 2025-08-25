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

// claude.ai fixed this, my attempt can be found 
// https://github.com/Landixbtw/oshell/blob/149c2aca4c62155b90a0b3952debf70ccea3c172/src/parsing.c

// TODO: UNDERSTAND THIS AND/OR REIMPLEMENT THIS MYSELF

// start_arg is the whole argument [i], the pos is the position of the char [i][j]
char *build_quote_string(char **arg, int start_arg, int start_pos, int end_arg, int end_pos, char quote) {
    // Calculate total length needed
    int total_length = 0;
    
    for(int i = start_arg; i <= end_arg; i++) {
        int start_j = (i == start_arg) ? start_pos + 1 : 0;  // Skip opening quote only in first arg
        int end_j = (i == end_arg) ? end_pos : strlen(arg[i]); // Stop at closing quote only in last arg
        
        for(int j = start_j; j < end_j; j++) {
            total_length++;
        }
        
        // Add space between arguments (except after the last one)
        if(i < end_arg) {
            total_length++; // for space
        }
    }

    char *result = malloc(total_length + 1);
    int index = 0;

    // Build the string
    for(int i = start_arg; i <= end_arg; i++) {
        int start_j = (i == start_arg) ? start_pos + 1 : 0;  // Skip opening quote only in first arg
        int end_j = (i == end_arg) ? end_pos : strlen(arg[i]); // Stop at closing quote only in last arg

        for(int j = start_j; j < end_j; j++) {
            result[index++] = arg[i][j];
        }

        // Add space between arguments (except after the last one)
        if(i < end_arg) {
            result[index++] = ' ';
        }
    }

    result[index] = '\0';
    return result;
}

// NOTE: Reason I am passing the array, and not just one string at a time is because I am saving the quote, 
// and doing this with passing a string would mean I have to safe the quote char globally no?
// this way it can be saved in the function, because we execute the function once
char **remove_quotes(char **arg) {
    size_t arg_count = 0;
    while(arg && arg[arg_count]) arg_count++;
    char **new_args = malloc((arg_count + 1) * sizeof(char*));
    
    // Initialize the last element to NULL
    new_args[arg_count] = NULL;
    
    bool *processed = calloc(arg_count, sizeof(bool)); // Track which args are processed
    
    for(int i = 0; i < arg_count; i++) {
        if (processed[i]) {
            continue; // Skip already processed arguments
        }
        
        bool found_quote = false;
        
        for(int j = 0; j < strlen(arg[i]); j++) {
            if ((arg[i][j] == '"' || arg[i][j] == '\'')) {
                char quote = arg[i][j];
                int start_arg = i;
                int start_pos = j;
                int end_arg = -1;
                int end_pos = -1;
                
                // Search for closing quote
                int current_i = i;
                int current_j = j + 1;
                
                bool found_closing = false;
                while (current_i < arg_count && arg[current_i] != NULL) {
                    while (current_j < strlen(arg[current_i])) {
                        if (arg[current_i][current_j] == quote) {
                            end_arg = current_i;
                            end_pos = current_j;
                            found_closing = true;
                            break;
                        }
                        current_j++;
                    }
                    if (found_closing) break;
                    current_i++;
                    current_j = 0;
                }
                
                if (found_closing) {
                    // fprintf(stderr, "start arg and pos: %c - %i %i\n", arg[start_arg][start_pos], start_arg, start_pos);
                    // fprintf(stderr, "end arg and pos: %c - %i %i\n", arg[end_arg][end_pos], end_arg, end_pos);
                    
                    // Build the quoted string and store it in new_args[i]
                    new_args[i] = build_quote_string(arg, start_arg, start_pos, end_arg, end_pos, quote);
                    
                    // Mark all arguments from start_arg to end_arg as processed
                    for(int k = start_arg; k <= end_arg; k++) {
                        processed[k] = true;
                    }
                    
                    found_quote = true;
                    break; // Exit the j loop since we processed this argument
                } else {
                    fprintf(stderr, "Warning: No closing quote found for quote starting at arg[%d][%d]\n", start_arg, start_pos);
                    break; // Don't continue looking for more quotes in this arg
                }
            }
        }
        
        // If no quote was processed, copy the original string
        if (!found_quote) {
            new_args[i] = malloc(strlen(arg[i]) + 1);
            strcpy(new_args[i], arg[i]);
        }
    }
    
    // Compact the array - remove NULL entries from skipped arguments
    int write_index = 0;
    for(int i = 0; i < arg_count; i++) {
        if (new_args[i] != NULL) {
            if (write_index != i) {
                new_args[write_index] = new_args[i];
                new_args[i] = NULL;
            }
            write_index++;
        }
    }
    new_args[write_index] = NULL; // Null terminate the compacted array
    
    free(processed);
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
