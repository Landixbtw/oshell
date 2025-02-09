#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/utils.h"

// this gives the length of array
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))



int main(void){
    for(;;) {
        // everything happens in here
        // if something happens a child process will be created with fork()
        //
        // for the MVP
        // get command 
        // parse it
        // execute it

        // FIX: Why is > printed if there are multiple lines of output ?, how can this be limited to one

        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        fprintf(stdout, "%s > ", cwd);
        fflush(stdout);
        // FIX: More error handling ie 
        // buffer overflow
        // is the while loop for fgets needed ? ie will there ever be more then one \n in a command

        oshell_loop();
    }
    return 0;
}

