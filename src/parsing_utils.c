#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>


// TODO: Set errno values on error

char **parse(char *input)
{

    size_t num_tokens = 0;
    while (input != NULL)
    {
        if (isblank(input))
        {
            num_tokens++;
        }
    }

    // pointer to a pointer -- represents an array of strings 
    char **args = malloc(sizeof(char *) * num_tokens);
    
    int i = 0;

    // split the input string everytime there is a space 
    char *token = strtok(input, " ");
    while (token != NULL)
    {
        // increments post storing the token is args
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL; 

    // free(args);

    return args;
}


bool is_valid_command(const char *command) {
    // Check against built-in commands
    const char *valid_commands[] = {"exit\n", "ping\n", "help\n", };
    for (int i = 0; i < sizeof(valid_commands) / sizeof(valid_commands[0]); i++) {
        if (strcmp(command, valid_commands[i]) == 0) {
            return true;
        }
    }
    return false;
}

void execute_command(char *input, char **args) {
    char *command = strtok(input, " ");
    if (is_valid_command(command)) {
        // Handle built-in command
    } else {
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
            int res = execvp(args[0], args);
            if (res == -1) perror("execvp()"); exit(EXIT_FAILURE);
        } else {
            fprintf(stderr, "fork() failed.\n");
            // set errno
            free(parsed);
            exit(EXIT_FAILURE);
        }
        free(parsed);
    }
}

