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
        return 0;
    }
    if (strcmp(args[0], "help\n") == 0) 
    {
        show_usage();
    }

    // FIX: These have to be desinged more compact this is basically the same code but with \n this is stupid

    // cd -> CD function
    if (args[0] != NULL && args[1] != NULL && strcmp("cd", command) == 0) 
    {
        system("pwd");
        int status = change_directory(args[1]); // the function handles sets errno value
        if (status != 0) perror("change_directory() Error"); return 1;
        system("pwd");
        return 0;
    } else if (args[0] != NULL && args[1] == NULL && strcmp("cd\n", command) == 0) {
        printf("Navigating to home dir.\n");
        // handle no cd input --> to home dir 
        return 0;
    }

    // kill -> kill function 
    if (args[0] != NULL && args[1] != NULL && strcmp("kill", command) == 0) {
        int status = kill_by_name(args[1]); // the function handles sets errno value
        if (status != 0) perror("kill_by_name() Error");
        return 0;
    } else if (args[1] == NULL && strcmp("kill\n", command) == 0) {
        printf("Please give a process to kill.\n");
        return 1;
    }

    // pipe -> something weird

// Execute as a system args[0]
    char **parsed = parse(args[0]);

    pid_t pid = fork();
    if (pid == 0)
    {
        // child
        waitpid(pid, NULL, 0);
    } else if (pid > 0){
        // Parent
        // args[0] is the args[0], the rest is the arguments 
        // int res = execvp(args[0], args);
        // if (res == -1) perror("execvp()"); exit(EXIT_FAILURE);
    } else {
        fprintf(stderr, "fork() failed.\n");
        // set errno
        free(parsed);
        exit(EXIT_FAILURE);
    }
    free(parsed);
    return 0;
}

