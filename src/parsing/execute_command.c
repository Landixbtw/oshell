#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <complex.h>
#include <assert.h>

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

    // cd -> CD function - utils.c
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

    // kill -> kill function - utils.c
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

    pid_t pid;
    int status;

    // FIX: there is always one letter missing

    size_t scmd_len = strlen("usr/bin/") + strlen(args[0]) + 1;
    printf("scmd_len: %ld\n", scmd_len); // prints 11
    char *scmd = malloc(scmd_len);
    assert(scmd);
    snprintf(scmd, scmd_len ,"/usr/bin/%s", args[0]);
    printf("scmd length: %ld\n", strlen(scmd)); // prints 10
    printf("command: %s\n", scmd);

    if ((pid = fork()) < 0)
        perror("oshell: fork() error");
    else if (pid == 0) {
        //child 
        printf("command: %s %s %s\n", args[0], args[1], args[2]);
        int res = execv(scmd, args);
        if (res == -1) {
            perror("execv() failed");
            exit(EXIT_FAILURE);
        }
        sleep(5);
        exit(1);
    }
    else do {
        if ((pid = waitpid(pid, &status, WNOHANG)) == -1)
            perror("oshell: waitpid() error");
        else if (pid == 0) {
            printf("child is still running.\n");
            sleep(1);
        } else {
            if (WIFEXITED(status))
            printf("child exited with status of %d\n", WEXITSTATUS(status));
            else puts("child did not exit succesfully");
        }
   } while (pid == 0);
    return 0;
}

