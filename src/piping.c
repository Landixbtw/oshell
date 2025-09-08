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

    int max_args[pipes_amount+1];
    memset(max_args, 0, sizeof(max_args));

    int start = 0;
    int end = 0;
    int cmd_id = 0;

    for(int i = 0; args[i] != NULL;i++) {
        if(strcmp(args[i], "|") == 0) {
            cmd_id++;
        } else {
            max_args[cmd_id]++;
        }
    }

    for (cmd_id = 0; cmd_id <= pipes_amount; cmd_id++) {
        if(cmd_id < pipes_amount) {
            commands[cmd_id] = malloc((max_args[cmd_id] + 1) * sizeof(char*));
            if (!commands[cmd_id]) {
                perror("oshell: memory allocation for command[cmd_id] failed");
            }
            memset(commands[cmd_id], 0, max_args[cmd_id] * sizeof(char*));

            end = pipe_pos[cmd_id];
            // create the commands from [cmd_id][i] = args[i]
            for(int i = 0; i < end; i++) {
                /*
                 * Since commands[cmd_id][i] is not initialized and only
                 * command[cmd_id] we need to allocate for each string in the array
                 * */
                if(args[i] != NULL) {
                    commands[cmd_id][i - start] = my_strdup(args[i]);
                    fprintf(stderr, "Storing: cmd=%d arg=%d addr=%p str=%s\n",
                    cmd_id, i - start, (void*)commands[cmd_id][i - start], commands[cmd_id][i - start]);
                }
                                }
            commands[cmd_id][end] = NULL;
        } else {
            commands[cmd_id] = malloc(max_args[cmd_id] * sizeof(char*));
            if (!commands[cmd_id]) {
                perror("oshell: memory allocation for command[cmd_id] failed");
            }
            memset(commands[cmd_id], 0, max_args[cmd_id] * sizeof(char*));

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
                    // i - start is the offset to not have a | included
                    commands[cmd_id][(i - start)] = my_strdup(args[i]);
                    fprintf(stderr, "Storing: cmd=%d arg=%d addr=%p str=%s\n",
                    cmd_id, i - start, (void*)commands[cmd_id][i - start], commands[cmd_id][i - start]);
                }
            }
            commands[cmd_id][end - start] = NULL;
        }
        size_t path_length = strlen("/usr/bin/") + strlen(commands[cmd_id][0]) + 1;
        paths[cmd_id] = malloc(path_length);
        if (!paths[cmd_id]) { perror("oshell: piping()"); exit(-1);}
        snprintf(paths[cmd_id], path_length, "/usr/bin/%s", commands[cmd_id][0]);

        for(int i = 0; i < max_args[cmd_id]; i++) {
            fprintf(stderr, "command[%i] -> [%i]: %s -- pointer: %p -- hex dump: ", cmd_id, i, commands[cmd_id][i], (void*)commands[cmd_id][i]);
            print_hex_dump(commands[cmd_id][i], strlen(commands[cmd_id][i]) + 1);
        }
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
		fprintf(stderr, "This is a command in the middle MULTIPIPE\n");
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
        pid_t result = wait(&status);  // Non-blocking wait
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

    for (int i = 0; i <= pipes_amount; i++) {
        for (int j = 0; commands[i][j]; j++) {
            free(commands[i][j]);
        }
        free(commands[i]);
        free(paths[i]);
    }

    free(paths);
    free(commands);

    return 0;
}
