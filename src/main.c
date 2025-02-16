#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/utils.h"

// this gives the length of array
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

int main(void){
    for(;;) {
        // TODO: infront of the wd should be user@machine e.g ole@tux
        //
        // TODO: This gets redirected to a file if the user does output 
        // redirection and then the console has no prompt
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        // we print all the "constant" stuff, to stderr, because if you use 
        // > >> the stdout is sent to the file, so this will also be sent to the file
        // so all shell prompts messages etc that is not from the user is printed to stderr
        fprintf(stderr ,"user@machine\n");
        fprintf(stderr, " %s > ", cwd);
        fflush(stderr);

        oshell_loop();
    }
    return 0;
}

