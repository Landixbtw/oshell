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

char *build_quote_string(char **arg, int start_arg, int start_pos, int end_arg, int end_pos) 
{
    int total_length = 0;

    for(int i = start_arg; i < end_arg; i++) {

        /*
         * We only want to skip the first character in the first word, 
         * since this is the outer quote that we want to remove
         * */
        int start_j;
        if(i == start_arg) start_j = start_pos + 1;
        else start_j = 0;

        int end_j;
        if (i == end_arg) end_j = end_pos;
        else end_j = strlen(arg[i]);

        for(int j = start_j; j < end_j; j++) total_length++; // amount of characters 
        if(i < end_arg) total_length++; // add N (amount of args) - 1 spaces for between each word
    }

    char *result = malloc((sizeof(char*) * total_length)  + 1);
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

    // result[index] = '\0';
    fprintf(stderr, "quote string hex dump: ");
    print_hex_dump(result, strlen(result) + 1);
    fprintf(stderr, "\n");
    return result;
}


// NOTE: Reason I am passing the array, and not just one string at a time is because I am saving the quote, 
// and doing this with passing a string would mean I have to safe the quote char globally no?
// this way it can be saved in the function, because we execute the function once
char **remove_quotes(char **arg) {
    size_t arg_count = 0;
    int quote_string_pos = 0;
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

                    // Build the quoted string and store it in new_args[i]
                    new_args[i] = build_quote_string(arg, start_arg, start_pos, end_arg, end_pos);
                    quote_string_pos = i;
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
         * NOTE: 
         * This !found_quote is working just fine but building the string together, 
         * with the quote string does not seem to work.
         * */

        // If no quote was processed, copy the original string
        if (!found_quote) {
            new_args[i] = malloc(strlen(arg[i]) + 1);
            strcpy(new_args[i], arg[i]);
        }
    }
    
    int write_index = 0;

    /*
     * NOTE: 
     * So this should go and look, if new_args[i] is "empty" and if yes, put in the string
     * that was just built, and assinged to [i], but only if there actually was something, 
     * checking for NULL does not work, because WHY? Printing the hex dump of the 
     * 'result' string with 
     *  result[index] = '\0';
     *  always prints 00 at the end, why?
     *  shouldnt it not print 00 if its not NULL terminated since there is no function used
     *  that does terminate?
     *      so there now needs to be a way, to find the spot, where it should go!
     *  But we cant just save the int, because if there are multiple it wont work?
     *  but checking for NULL also throws error, and going i < arg_count also does not 
     *  work. The if statement never triggers because the loop leaves right when 
     *  the new quote string would be placed there
     *      This write_index idea does not work. There is a better approach. BUT WHAT.
     * */


    for(int i = 0; new_args[i] != NULL; i++) {
    fprintf(stderr, "index: %i\n", i);
    fprintf(stderr, "write_index: %i\n", write_index);
            if (write_index != i) {
                fprintf(stderr, "new_args: %s", new_args[i]);
                strcpy(new_args[write_index], new_args[i]);
                new_args[i] = NULL;
            }
            write_index++;
        }
    new_args[write_index] = NULL; // Null terminate the compacted array

    free(processed);
    return new_args;
}
