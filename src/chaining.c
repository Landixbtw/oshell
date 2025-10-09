/*
 * command chaining &&
 * */

#include "../include/chaining.h"


// TODO: do research on chaining and try to implement this. this should prob
// be called while parsing if && is detected, take input and return n seperate 
// commands that are to be executed indepentend.

/*
 * mkdir testdir && cd testdir && touch datei.txt 
 * ping -c 1 google.com && echo "Netzwerk OK"
 * uname -a && cat /etc/os-release && free -h && df -h && uptime
 uname -a && cat /etc/os-release && free -ö && df -h && uptime
 *
 * chainig with && (AND) only execute the next if the previous one succeeded
 * this means we have to always check for the return of the previous command
 * to "approve" the next one
 *
 * */



char **split_on_chain(char *input) {
    // we want to split the input on && before we pass it to the parse function so that 
    // they are treated as n seperate commands for the execution to work.
    // this would be a hassle to do it in the parse function.
    
    /*
     * && has to be detected as "this is a new command" before the tokenize function
     * and then the input has to be "split"
     * */

    char **output = malloc(sizeof(char*) * strlen(input));

    // NOTE: there probably is a better solution to detect && but this seems to work
    for(int i = 0; i < strlen(input); i++) {
        for(int j = 0; output[i] != NULL; j++) {
            if(input[i] == '&' && input[i+1] == '&') {
                // the input needs to be split into two commands
                // next array field
                j++;
            } else {
               output[i][j] = input[i];
            }
        }
    }

    

    fprintf(stderr, "output: %s\n", output[0]);
    return output;
}
