/*
 *  https://linuxhandbook.com/redirection-linux/
 *  https://beej.us/guide/bgipc/html/split-wide/pipes.html#pipes https://claude.ai/chat/77ae4eba-3cc6-440e-9aaa-4d13d97fac76
 *  https://stackoverflow.com/questions/2804217/using-pipes-in-linux-with-c
 *  pipe redirection
 *
 *  pipe redirection: |
 *
 *
 *  command | different command
 *
 * */

#include "../include/piping.h"
#include "../include/parsing_utils.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>


// instead of having to allocate everytime in a loop one can just call this function with the string
char *my_strdup(const char *s) {
    size_t len = strlen(s) + 1; 
    char *copy = malloc(len);
    if (copy) {
        strcpy(copy, s);
    }
    return copy;
}


int pipe_redirection(char **args) 
{
/*
     * To support more then one, pipe for "advanced" commands, there needs to be a way to track the amount of pipes we have
     * for later redirection, tracking of the multiple pipe positions, and the spliting of the commands
     * */
    int pipes_amount = 0;

    for (int i = 0; args[i] != NULL; i++) {
        if(strcmp("|", args[i]) == 0) pipes_amount++;
    }


    if(pipes_amount == 0) return -1; // no pipes found, however

    
    int pipe_pos[pipes_amount];
    int pipe_index = 0;
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "|") == 0) {
            pipe_pos[pipe_index++] = i;
        }
    }

    char ***commands = malloc((pipes_amount + 1) * sizeof(char**));
    char **paths = malloc((pipes_amount + 1) * sizeof(char*));

    /*
     * So a char ***foobar is a pointer to an array of pointers that point to pointers to char.
     * It's essentially a pointer to an array of argv arrays.
     *
     * commands (char***)
     *      │
     *      └──→ [0] ──→ argv for "echo hello"     ──→ [0]──→"echo"
     *           [1] ──→ argv for "wc -w"          ──→ [1]──→"hello"
     *                                                 [2]──→ NULL
     *
     *                                                 [0]──→"wc" 
     *                                                 [1]──→"-w"
     *                                                 [2]──→ NULL
     * Memory layout:
     * commands ──→ ┌─────────┐
     *              │ ptr[0]  │──→ ┌─────────┐
     *              ├─────────┤    │"echo"   │
     *              │ ptr[1]  │    │"hello"  │
     *              └─────────┘    │ NULL    │
     *                   │         └─────────┘
     *                   └──────→ ┌─────────┐
     *                            │"wc"     │
     *                            │"-w"     │
     *                            │ NULL    │
     *                            └─────────┘
     *
     * Usage: execv(path, commands[0]) for first command
     *        execv(path, commands[1]) for second command
     */

    int MAX_ARGS = 0;
    for(int i = 0; args[i] != NULL; i++) {
        MAX_ARGS++;
    }

    int start = 0;
    int end = 0;
    int cmd_id = 0;
    for (cmd_id = 0; cmd_id <= pipes_amount; cmd_id++) {
        if(cmd_id < pipes_amount) {

            commands[cmd_id] = malloc(MAX_ARGS * sizeof(char*));
            if (!commands[cmd_id]) {
                perror("oshell: memory allocation for command[cmd_id] failed");
            }
            memset(commands[cmd_id], 0, MAX_ARGS * sizeof(char*));

            end = pipe_pos[cmd_id];
            // create the commands from [cmd_id][i] = args[i]
            for(int i = 0; i < end; i++) {
                /*
                 * Since commands[cmd_id][i] is not initialized and only
                 * command[cmd_id] we need to allocate for each string in the array
                 * */
                if(args[i] != NULL) {
                    commands[cmd_id][i] = malloc(strlen(args[i]) + 1);
                    strcpy(commands[cmd_id][i], args[i]);
                }
                                }
            commands[cmd_id][end] = NULL;
        } else {
            commands[cmd_id] = malloc(MAX_ARGS * sizeof(char*));
            if (!commands[cmd_id]) {
                perror("oshell: memory allocation for command[cmd_id] failed");
            }
            memset(commands[cmd_id], 0, MAX_ARGS * sizeof(char*));

            int j = 0;
            do {
                j++;
            }while (args[j] != NULL);
            start = end + 1; // Skip the pipe
            end = j;
            for(int i = start; i < end; i++) {
                // using strdup we dont need to manually allocate memory for 
                // commands[cmd_id][xyz] since strdup handles that
                if(args[i] != NULL) {
                    // shit fuck why is i - start needed ??? DOES NOT WORK WITHOUT
                    commands[cmd_id][(i - start)] = my_strdup(args[i]);
                }
            }
            commands[cmd_id][end - start] = NULL;
        }
        size_t path_length = strlen("/usr/bin/") + strlen(commands[cmd_id][0]) + 1;
        paths[cmd_id] = malloc(path_length);
        if (!paths[cmd_id]) { perror("oshell: piping()"); exit(-1);}
        snprintf(paths[cmd_id], path_length, "/usr/bin/%s", commands[cmd_id][0]);        

        for(int i = 0; i < MAX_ARGS; i++) {
            fprintf(stderr, "command[%i] -> [%i]: %s\n", cmd_id, i, commands[cmd_id][i]);
        }
    }
 
    /*
     *echo "foo bar baz" | wc -w
	arg: echo - hex dump: 65 63 68 6F 00
	arg: "foo - hex dump: 22 66 6F 6F 00
	arg: bar - hex dump: 62 61 72 00
	arg: baz" - hex dump: 62 61 7A 22 00
	arg: | - hex dump: 7C 00
	arg: wc - hex dump: 77 63 00
	arg: -w - hex dump: 2D 77 00
quoted string [1]: foo bar baz
commands[0][0] echo - hex dump: 65 63 68 6F 00
commands[0][1] foo bar baz - hex dump: 66 6F 6F 20 62 61 72 20 62 61 7A 00

paths[2]: (null)
foo bar baz | wc -w
foo bar baz | wc -w
	arg: foo - hex dump: 66 6F 6F 00
	arg: bar - hex dump: 62 61 72 00
	arg: baz - hex dump: 62 61 7A 00
	arg: | - hex dump: 7C 00
	arg: wc - hex dump: 77 63 00
	arg: -w - hex dump: 2D 77 00
commands[0][0] foo - hex dump: 66 6F 6F 00
commands[0][1] bar - hex dump: 62 61 72 00
commands[0][2] baz - hex dump: 62 61 7A 00

     * */

    /*
     * pipe() returns two file descriptors, fd[0] is open for reading, fd[1] is open for writing
     * ouput of fd[1] is input for fd[0].
     *
     * Since pipes are a point-to-point connection, one for every command, so we need n-1 pipes if n is the amount of commands we have
     * */

    int fd[pipes_amount][2];

    for(int i = 0; i < pipes_amount; i++) {
        if (pipe(fd[i]) == -1) {
            perror("oshell: fd ");
        }
    }

    /*
     * We need to track, what gets redirected where
     * "This is the first command, only redirect STDOUT"
     * "This is a middle command, redirect both"
     * "This is the last command, only redirect STDIN"
     * */

    // save the stdin/stdout, to later restore 
    int saved_stdout = dup(STDOUT_FILENO);
    int saved_stdin = dup(STDIN_FILENO);

    int f = 0;
    for(int i = 0; i < cmd_id; i++) {
        if ((f = fork()) == 0) {
            // child: setup the redirections
            if(i == 0) { // first command, only redirect stdout
                if(dup2(fd[i][1], STDOUT_FILENO) == -1) {
                    fprintf(stderr, "Child %d: dup2 failed\n", i);
                    _exit(EXIT_FAILURE);
                }
                for(int j = 0; j < pipes_amount; j++) {
                    close(fd[j][0]);
                    close(fd[j][1]);
                }
            } else if (i == cmd_id - 1) { // last command only redirect stdin
                if(dup2(fd[i-1][0], STDIN_FILENO) == -1) {
                    close(fd[i-1][0]);
                    _exit(EXIT_FAILURE);
                }
                for(int j = 0; j < pipes_amount; j++) {
                    close(fd[j][0]);
                    close(fd[j][1]);
                }
            } else { // not first nor last, redirect stdin and stdout
                     // cmd0 ---> pipe[0] ---> cmd1 ---> pipe[1] ---> cmd2
                if(dup2(fd[i-1][0], STDIN_FILENO) == -1 || dup2(fd[i][1], STDOUT_FILENO) == -1) {
                    _exit(EXIT_FAILURE);
                }
                for(int j = 0; j < pipes_amount; j++) {
                    close(fd[j][0]);
                    close(fd[j][1]);
                }
            }
            if(commands[i] == NULL) {
                fprintf(stderr, "ERROR: commands[%d] is NULL\n", i);
                _exit(EXIT_FAILURE);
            }

            if (execv(paths[i], commands[i]) == -1) {
                perror("execv() failed");
                free(paths);
                free(commands);
                for(int i = 0; i < pipes_amount; i++) {
                    close(fd[i][0]);
                    close(fd[i][1]);
                }
                _exit(EXIT_FAILURE);
            }
            fprintf(stderr, "Child %d: ERROR - this should never print!\n", i);
            } else if (f == -1) {
                perror("oshell: fork() failed"); 
                return -1; 
            }
            // parent continues to next iteration
        }
    for(int j = 0; j < pipes_amount; j++) {
        close(fd[j][0]);
        close(fd[j][1]);
    }

    for(int i = 0; i < cmd_id; i++) {
        int status;
        pid_t result = waitpid(-1, &status, WNOHANG);  // Non-blocking wait
        if (result == 0) {
            continue;
        } else if (result > 0) {
            // fprintf(stderr, "Child %d finished with status %d\n", result, status);
        }
    }

    // restore stdin/stdout
    dup2(saved_stdin, STDIN_FILENO);
    dup2(saved_stdout, STDOUT_FILENO);

    if(close(saved_stdin) != 0) perror("oshell: piping()");
    if(close(saved_stdout) != 0) perror("oshell: piping()");

    free(paths);
    free(commands);

    return 0;
}
