#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


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
                    new_args[i] = build_quote_string(arg, start_arg, start_pos, end_arg, end_pos);
                    
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
        
        // If no quote was processed, copy the original string
        if (!found_quote) {
            new_args[i] = malloc(strlen(arg[i]) + 1);
            strcpy(new_args[i], arg[i]);
        }
    }
    
    // Compact the array - remove NULL entries from skipped arguments
    int write_index = 0;
    for(int i = 0; i < arg_count; i++) {
        if (new_args[i] != NULL) { // FIX: Conditional jump or move depends on uninitialised value(s), from malloc
                                   // they seem to not be init only memory allocated
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
