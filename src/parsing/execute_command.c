#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <complex.h>

#include "../../include/parse.h"
#include "../../include/utils.h"


int execute_command(char **args) 
{
    char *command = strtok(args[0], " ");

    // Handle built-in args[0]
    if (strcmp(args[0], "exit\n") == 0) 
    {
        printf("Exiting Shell...\n");
        exit(EXIT_SUCCESS);
    }
    if (strcmp(args[0], "help\n") == 0) 
    {
        show_usage();
        return 0;
    }

    // FIX: These have to be desinged more compact this is basically the same code but with \n this is stupid

    // cd -> CD function
    if (args[0] != NULL && args[1] != NULL && strcmp("cd", command) == 0) 
    {
        int status = change_directory(args[1]);
        if (status != 0) {
            perror("change_directory() Error");
            return 1;
        }
        return 0;
    } else if (args[0] != NULL && args[1] == NULL && strcmp("cd\n", command) == 0) {
        chdir(getenv("HOME"));
        return 0;
    }

    // kill -> kill function 
    if (args[0] != NULL && args[1] != NULL && strcmp("kill", command) == 0) {
        int status = kill_process(args[1]); // the function handles sets errno value
        if (status != 0) perror("kill_by_name() Error");
        return 0;
        // FIX: This does not work
    } else if (args[0] != NULL && args[1] == NULL && strcmp("kill\n", command) == 0) {
        printf("Please give a process to kill.\n");
        return 1;
    }

    // pipe -> | function

    pid_t pid = fork();
    if (pid == 0)
    {
        // child
        waitpid(pid, NULL, 0);
    } else if (pid > 0){
        // Parent
        // args[0] is the args[0], the rest is the arguments 
        // FIX: This is not working, 

        int res = execvp(args[0], args);
        if (res == -1) {
            perror("execvp() failed");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "fork() failed.\n");
        // set errno
        exit(EXIT_FAILURE);
    }
    return 0;
}

