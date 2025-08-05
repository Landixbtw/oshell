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


#include "../include/Header.h"

int pipe_redirection(char **args)
{
    /*
     * To support more then one, pipe for "advanced" commands, there needs to be a way to track the amount of pipes we have
     * for later redirection, tracking of the multiple pipe positions, and the spliting of the commands
     * */
    int pipes_amount = 0;

    for (int i = 0; i < sizeof(args); i++) {
        if(strcmp("|", args[i]) == 0) pipes_amount++;
    }

    if(pipes_amount == 0) return -1; // no pipes found, however

    int pipe_pos[pipes_amount] = {};
    int _ = 0;
    while(args[pipe_pos[_]] != NULL && _ < pipes_amount) {
        if(strcmp("|", args[pipe_pos[_]]) != 0) pipe_pos[_]++;
        else _++;
    }


    // this splits the args into two command
    for(int i = 0; i < pipes_amount; i++) {
        args[pipe_pos[i]] = NULL;
    }

    char **cmd1 = args;
    // cmd2 needs to be pipe_pos + 1 until == NULL
    char **cmd2 = malloc(sizeof(args));

    for(int i = 0; i < sizeof(args); i++) {
        if(args[pipe_pos + i] != NULL) {
            cmd2[i] = args[pipe_pos + i];
        }
    }


    char *outputCmd1 = malloc(sizeof(cmd1));
    char *outputCmd2 = malloc(sizeof(cmd2));

    int fd[2];
    
    int res;

    /*
     * pipe() returns two file descriptors, fd[0] is open for reading, fd[1] is open for writing
     * ouput of fd[1] is input for fd[0].
     * */
    res = pipe(fd);
    if (res != 0) {
        return res;
    }

    // im pretty sure I dont need to set anything here
    // fcntl(fd[0], F_SETFD, FD_CLOEXEC); // read end 
    // fcntl(fd[1], F_SETFD, FD_CLOEXEC); // write end 

    /*
     * NOTE: We want to connect the stdout of cmd1 with the stdin of ie cmd2[1] grep
     * so this would be default, fd[1] to fd[0] ?
     * */

    size_t path_length = strlen("/usr/bin/") + strlen(cmd1[0]) + 1;
    char *path = malloc(path_length);
    snprintf(path, path_length, "/usr/bin/%s", cmd1[0]);

    char **argv;

    int k = 0;
    while (cmd1[k + 1] != NULL) {
        k++;
    }

    argv = malloc((k + 1) * sizeof(char*));

    // k gives number of arguments
    for(int j = 0; j < k; j++) {
        argv[j] = cmd1[j + 1];
    }

    argv[k] = NULL;

    // for N commands we N processes and N-1 pipes

    /*
     * Create pipe (empty)
     * Fork for echo 
     * Fork for grep 
     * In echo's process: redirect STDOUT to pipe's write end 
     * In grep's process: redirect STDIN to pipe's read end 
     * NOW both processes call execv()
     * */

    // number of commands
    // TODO: This needs to be dynamically
    int n = 2;


    /*
     * We need to track, what gets redirected where
     * "This is the first command, only redirect STDOUT" 
     * "This is a middle command, redirect both" 
     * "This is the last command, only redirect STDIN"
     * */

    for(int i = 0; i < n; i++) {
        if (fork() == 0) {
            // child: setup the redirections
            if(dup2(fd[0], STDIN_FILENO) == -1) {
                exit(EXIT_FAILURE);
            }

            close(fd[1]);
            if (execv(path, argv) == -1) {
                perror("execv() failed");
                free(path);
                free(argv);
                exit(EXIT_FAILURE);
            }
        }
        // parent continues to next iteration
    }

    // switch (fork()) {
    //     case -1: // handle error
    //         close(fd[0]);
    //         close(fd[1]);
    //         break;
    //     case 0: // child reads from pipe
    //         close(fd[1]); // write end is unused
    //         // read from fd into buf
    //         // buf, is empty cause there is nothing to read from, so we first have to exec, 
    //         // cmd1?
    //         if (read(fd[0], outputCmd1, sizeof(cmd1)) <= 0) {
    //         
    //         }
    //         if(dup2(fd[0], STDOUT_FILENO) == -1) {
    //             exit(EXIT_FAILURE);
    //         }
    //     default: // parent writes to pipe
    //         close(fd[0]); // read end is unused
    //         // 'input' would write the second command ie grep xxx
    //         if (write(fd[1], cmd2, sizeof(args[2])) <= 0) {
    //             for(int i = 0; i < sizeof(cmd2); i++) {
    //                 if(cmd2[i] != NULL)
    //                     fprintf(stderr, "cmd2: %s\n", cmd2[i]);
    //             }
    //         }
    //         if(dup2(fd[1], STDIN_FILENO) == -1) {
    //             exit(EXIT_FAILURE);
    //         }
    // }

    close(fd[0]);
    close(fd[1]);

    return 0;
}
