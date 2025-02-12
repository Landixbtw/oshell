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
        // TODO: infront of the wd should be user@machine e.g ole@tux
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        printf("user@machine:\n");
        printf("  %s > ", cwd);
        fflush(stdout);

        oshell_loop();
    }
    return 0;
}

