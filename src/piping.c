/*
 *  https://linuxhandbook.com/redirection-linux/
 *  https://beej.us/guide/bgipc/html/split-wide/pipes.html#pipes
 *  https://claude.ai/chat/77ae4eba-3cc6-440e-9aaa-4d13d97fac76
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>



void print_hex_dump(const char* data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        fprintf(stderr, "%02X ", (unsigned char)data[i]);
        if ((i + 1) % 16 == 0) fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
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
    int cmd_idx = 0;
    for (; cmd_idx <= pipes_amount; cmd_idx++) {
        if(cmd_idx < pipes_amount) {
            end = pipe_pos[cmd_idx];
            // create the commands from [cmd_idx][i] = args[i]
            for(int i = 0; i < end; i++) {
                /*
                 * Since commands[cmd_idx][i] is not initialized and only
                 * command[cmd_idx] we need to allocate for each string in the array
                 * */
                commands[cmd_idx] = malloc(MAX_ARGS * sizeof(char*));
                if(args[i] != NULL) {
                    commands[cmd_idx][i] = malloc(strlen(args[i]) + 1);
                    strcpy(commands[cmd_idx][i], args[i]);
                    fprintf(stderr, "commands[%i][%i] %s\n", cmd_idx, i,commands[cmd_idx][i]);
                    print_hex_dump(commands[cmd_idx][i], strlen(commands[cmd_idx][i]) + 1);
                }
                                }
            commands[cmd_idx][end] = NULL;
        } else {
            int j = 0;
            do {
                j++;
            }while (args[j] != NULL);
            start = end + 1; // Skip the pipe
            end = j;
            for(int i = start; i < end; i++) {
                commands[i] = malloc(MAX_ARGS * sizeof(char*));
                if(args[i] != NULL)
                    commands[cmd_idx][i] = args[i];
            }
            commands[cmd_idx][end] = NULL;
        }
    }


    // if cmd_idx 0 is NULL this will crash and burn
    for(int i = 0; commands[cmd_idx][i] != NULL; i++) { // FIX: ../src/piping.c:122:37: runtime error: load of null pointer of type 'char *'
        fprintf(stderr, "commands\n");
        fprintf(stderr, "%s", commands[cmd_idx][i]);
        size_t path_length = strlen("/usr/bin/") + strlen(commands[cmd_idx][start]) + 1;
        paths[cmd_idx] = malloc(path_length);
        if (!paths[cmd_idx]) { perror("oshell: piping()"); exit(-1);}
        snprintf(paths[cmd_idx], path_length, "/usr/bin/%s", commands[cmd_idx][start]);
    }


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


    // TODO: We need to know what number of command we have, this does not seem to work
    // stdout does not get redirected to stdin of second command
    

    for(int i = 0; i < cmd_idx; i++) {
        if (fork() == 0) {
            // child: setup the redirections
            if(i == 0) { // first command, only redirect stdout
                if(dup2(fd[i][1], STDOUT_FILENO) == -1) {
                    return -1;
                }
                fflush(stdout);
                for(int j = 0; j < pipes_amount; j++) {
                    close(fd[j][0]);
                    close(fd[j][1]);
                }

            } else if (i == cmd_idx - 1) { // last command only redirect stdin
                if(dup2(fd[i-1][0], STDIN_FILENO) == -1) {
                    close(fd[i-1][0]);
                    return -1;
                }
                fflush(stdout);
                for(int j = 0; j < pipes_amount; j++) {
                    close(fd[j][0]);
                    close(fd[j][1]);
                }
            } else { // not first nor last, redirect stdin and stdout
                     // cmd0 ---> pipe[0] ---> cmd1 ---> pipe[1] ---> cmd2
                if(dup2(fd[i-1][0], STDIN_FILENO) == -1 || dup2(fd[i][1], STDOUT_FILENO) == -1) {
                    return -1;
                }
                for(int j = 0; j < pipes_amount; j++) {
                    close(fd[j][0]);
                    close(fd[j][1]);
                }
            }

            // this should +1 since we dont want 0 because 0 is the command in paths ie echo 
            if(commands[cmd_idx] == NULL) return -1;
            if (execv(*paths, commands[cmd_idx]) == -1) {
                perror("execv() failed");
                free(paths);
                free(commands);
                for(int i = 0; i < pipes_amount; i++) {
                    close(fd[i][0]);
                    close(fd[i][1]);
                }
                exit(EXIT_FAILURE);
            }

            // NOTE: This never gets printed
            fprintf(stderr, "%s", paths[0]);
            for(int i = 0; args[i] != NULL; i++) {
                fprintf(stderr, "%s", args[i]);
            }
        }
        // parent continues to next iteration
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
