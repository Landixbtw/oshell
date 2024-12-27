#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <complex.h>

#include "../../include/parse.h"
#include "../../include/utils.h"


void execute_command(char *input, char **args) 
{
    char *command = strtok(input, " ");
    // Handle built-in command
    if (strcmp(command, "exit\n") == 0) 
    {
        printf("Exiting Shell...\n");
        exit(EXIT_SUCCESS);
    }
    if (strcmp(command, "help\n") == 0) 
    {
        show_usage();
    }

    // cd -> CD function
    if (args[0] != NULL) 
    {
        printf("command arg: %s \n" ,args[1]);
        if (args[1] != NULL && strcmp(command, "cd"))
        {
            system("pwd");
            int status = change_directory(args[1]); // the function handles sets errno value
            if (status != 0) perror("change_directory() Error"); exit(EXIT_FAILURE);
            system("pwd");
            exit(EXIT_SUCCESS);
        } else if (args[1] == NULL){
            printf("this is triggered when there is no arg given to cd\n");
            // navigate to home dir with chdir 
            exit(EXIT_SUCCESS);
        }
    }

    // kill -> kill function 
    // if (args[0] != NULL) {
    //     if (strcmp(command, "kill")) {
    //         int status = kill_by_name(); // the function handles sets errno value
    //         if (status != 0) perror("kill_by_name() Error");
    //     }
    // }
    // pipe -> something weird

// Execute as a system command
    char **parsed = parse(input);

    pid_t pid = fork();
    if (pid == 0)
    {
        // child
        waitpid(pid, NULL, 0);
    } else if (pid > 0){
        // Parent
        // args[0] is the command, the rest is the arguments 
        printf("command: %s  args: %s\n", args[0], args[1]);
        // int res = execvp(args[0], args);
        // if (res == -1) perror("execvp()"); exit(EXIT_FAILURE);
    } else {
        fprintf(stderr, "fork() failed.\n");
        // set errno
        free(parsed);
        exit(EXIT_FAILURE);
    }
    free(parsed);
}

