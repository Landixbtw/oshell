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

        // TODO: only print this, when there is actuall input to be made, dont print this everytime
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        fprintf(stdout, "%s > ", cwd);
        fflush(stdout);

        oshell_loop();
    }
    return 0;
}

