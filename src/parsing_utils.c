#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/parsing_utils.h"

/*
 * This function takes in the an array of strings, with the start_arg representing the word [i]. The start_pos representing the char [j]
 * aswell as the end_arg and end_pos.
 *
 * This should loop from start_arg to start_pos, then determine the start_j and end_j.
 * We also need to calculate the length needed for the new string, therefore we just count the amount of characters.
 *
 * Then just build the string from start_j to end_j
 * */

// WARN: claude.ai fixed this, my attempt can be found 
// https://github.com/Landixbtw/oshell/blob/149c2aca4c62155b90a0b3952debf70ccea3c172/src/parsing.c

void print_hex_dump(const char* data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        fprintf(stderr, "%02X ", (unsigned char)data[i]);
        if ((i + 1) % 16 == 0) fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
}

char *build_quote_string(char **arg, const int start_arg, const int start_pos, const int end_arg, const int end_pos)
{
    int total_length = 0;

    for(int i = start_arg; i < end_arg; i++) {

        // this is needed otherwise, the code will try to execute the command provided e.g.
        // echo "foo bar baz" | wc -w just like that. 
        // this would output foo bar baz | wc -w /*


        /* We only want to skip the first character in the first word, 
         * since this is the outer quote that we want to remove
         * */
        int start_j;
        if(i == start_arg) start_j = start_pos + 1;
        else start_j = 0;

        int end_j;
        if (i == end_arg) end_j = end_pos;
        else end_j = strlen(arg[i]);

        for(int j = start_j; j < end_j; j++) total_length++;
        if(i < end_arg) total_length++; 
    }

    char *result = malloc((sizeof(char) * total_length)  + 1);
    if (!result) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    int index = 0;

    /*
     * Building the string
     * This is basically the same as the loop before we make start_j and end_j 
     * and then loop from start to end, adding the characters one by one to the 
     * result string and null terminating it.
     * */

    for(int i = start_arg; i <= end_arg; i++) {
        int start_j;
        if(i == start_arg) start_j = start_pos + 1;
        else start_j = 0;

        int end_j;
        if (i == end_arg) end_j = end_pos;
        else end_j = strlen(arg[i]);

        for(int j = start_j; j < end_j; j++) result[index++] = arg[i][j];

        // add a space after each word
        if(i < end_arg) {
            result[index++] = ' ';
        }
    }
    return result;
}


// NOTE: Reason I am passing the array, and not just one string at a time is because I am saving the quote, 
// and doing this with passing a string would mean I have to safe the quote char globally no?
// this way it can be saved in the function, because we execute the function once
// NOTE: A variable called whose value was assinged to remove_quotes has to be freed
char **remove_quotes(char **arg) {
    if (arg == NULL) {
        perror("remove_quotes failed");
    }

    size_t arg_count = 0;
    size_t new_arg_count = 0;
    int quote_string_pos = 0;
    while(arg && arg[arg_count]) arg_count++;
    // in this case calloc is being used, because there are cases where it is being 
    // accessed without having a value there causing it to crash.
    char **tmp_args = calloc(arg_count + 1, sizeof(char*));
    if(tmp_args == NULL) perror("tmp_args memory allocation failed ");

    bool *processed = calloc(arg_count, sizeof(bool)); // Track which args are processed

    // this is not declared with the others e.g. start_arg,start_pos 
    // because it is needed later down out of 
    // that scope
    int end_arg = -1;
    int end_pos = -1;

    for(int i = 0; i < arg_count; i++) {
        if (processed[i]) {
            continue; // Skip already processed arguments
        }
        if (arg[i] == NULL) {
            i++;
        }
        bool found_quote = false;

        for(int j = 0; j < strlen(arg[i]); j++) {
            if ((arg[i][j] == '"' || arg[i][j] == '\'')) {
                char quote = arg[i][j];
                int start_arg = i;
                int start_pos = j;

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
                    // Build the quoted string and store it in new_args[i]
                    tmp_args[i] = build_quote_string(arg, start_arg, start_pos, end_arg, end_pos);
                    // fprintf(stderr, "quoted string [%i]: %s\n", i, tmp_args[i]);

                    // Mark all arguments from start_arg to end_arg as processed
                    for(int k = start_arg; k <= end_arg; k++) {
                        processed[k] = true;
                    }

                    found_quote = true;
                    break; 
                } else {
                    fprintf(stderr, "Warning: No closing quote found for quote starting at arg[%d][%d]\n", start_arg, start_pos);
                    break; 
                }
            }
        }

        /*
         * TODO: / NOTE 
         * Since the array is shorther, because the string in between the quotes is now a single string and not 
         * multiple different ones, the string is terminated early basically looks like this.
         *      [echo] [foo bar baz] [NULL] [NULL] [|] [wc] [-w] instead of 
         *      [echo] [foo]         [bar]  [baz]  [|] [wc] [-w] 
         * This means the string has to be shortened and everything has to move by x amount to the left
         * - will memmove do the trick?
         * */


        /* THIS IS FOR THE PROJECT REPORT DETAILED EXAMPLE OF OUTPUT AT THIS STATE
         *  arg: echo - hex dump: 65 63 68 6F 00
        arg: foo bar baz - hex dump: 66 6F 6F 20 62 61 72 20 62 61 7A 00
         NULL
         NULL
        arg: | - hex dump: 7C 00
        arg: wc - hex dump: 77 63 00
        arg: -w - hex dump: 2D 77 00
        arg: foo bar baz - hex dump: 66 6F 6F 20 62 61 72 20 62 61 7A 00
         *
         *
         * */

        /*
         * NOTE: 
         * This !found_quote is working just fine but building the string together, 
         * with the quote string does not seem to work.
         * */


        // If no quote was processed, copy the original string
        if (!found_quote) {
            // fprintf(stderr, "no found quote [%i]: %s\n", i, arg[i]);
            // tmp_args[i] = malloc(strlen(arg[i]) + 1);
            // if(tmp_args[i] == NULL) fprintf(stderr,"oshell: memory allocation for %s failed", tmp_args[i]);
            // strcpy(tmp_args[i], arg[i]);
            tmp_args[i] = my_strdup(arg[i]);
        }

        // we want to memmove the string array entries now by count_diff to the left.
        // source should be end_args and dest should be end args - count diff?

        // for(int i = 0; i < arg_count; i++) {
        //     if(tmp_args[i] == NULL) {
        //         fprintf(stderr, "\t NULL \n");
        //     } else {
        //         fprintf(stderr, "\targ[%i]: %s - hex dump: ", i, tmp_args[i]);
        //         print_hex_dump(tmp_args[i], strlen(tmp_args[i]) + 1);
        //     }
        // }
    }
    int n = arg_count + 1;

    int j = 0;
    for (int i = 0; i < n; i++) {
        if (tmp_args[i] != NULL) {
            if (i != j) tmp_args[j] = tmp_args[i];
            j++;
        }
    }
    // Set leftover slots to NULL
    for ( ; j < n; j++) tmp_args[j] = NULL;

    free(processed);
    free(arg);
    return tmp_args;
}

// instead of having to allocate everytime in a loop one can just call this function with the string
char *my_strdup(const char *s) {
    size_t len = strlen(s) + 1; 
    char *copy = malloc(len);
    if (copy) {
        strcpy(copy, s);
    }
    return copy;
}


// we want to handle \ as \n not just text
// right now it is just text which does not allow us to chain commands
// together via linebreak since && is not implemented (yet)

/*
 * If \ is detected as a single char we want to linebreak 
 * we do not want to linebreak if \n is detected since it could 
 * be included in a string thta is supposed to go into a file or sometihng
 * and we do not want to mess with that. And mess up the input.
 * */


