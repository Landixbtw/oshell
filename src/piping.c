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

int _pipe(char **args)
{
    int pipe_pos = 0;
    while(args[pipe_pos] != NULL && strcmp("|", args[pipe_pos]) != 0) {
        pipe_pos++;
    }

    if (args[pipe_pos] == NULL) {
        fprintf(stderr, "No pipe symbol found.\n");
        return -1;
    }

    // this splits the args into two command
    args[pipe_pos] = NULL;
    
    char **cmd1 = args;
    // cmd2 needs to be pipe_pos + 1 until == NULL
    char **cmd2 = malloc(sizeof(args));

    for(int i = 0; i < sizeof(args); i++) {
        if(args[pipe_pos + i] != NULL) {
            cmd2[i] = args[pipe_pos + i];
        }
    }


    int fildes[2];
    const int BSIZE = 100;
    char buf[BSIZE];
    ssize_t nbytes = 0;

    if (pipe(fildes) == -1) {
        perror("ohshell: _pipe");
        return -1;
    }

    fcntl(fildes[0], F_SETFD, FD_CLOEXEC); // read end 
    fcntl(fildes[1], F_SETFD, FD_CLOEXEC); // write end 

    char *finished_command = "";

    /*
     * NOTE: We want to connect the stdout of cmd1 with the stdin of ie cmd2[1] grep
     * */
    
    switch (fork()) {
        case -1: // handle error
            perror("oshell: _pipe()");
            close(fildes[0]);
            close(fildes[1]);
            break;

        case 0: // child reads from pipe
            close(fildes[1]); // write end is unused
            nbytes = read(fildes[0], buf, BSIZE); // get data from the pipe
            // assert(nbytes <= 0);
            if (read(fildes[0], cmd1, sizeof(cmd1)) > 0)
                // 'buf' reads, the first command ie ls (ls | grep xxx)
                // puts("oshell: _pipe(): EOF or error detected.");
                perror("oshell: _pipe() read child");
            if(dup2(fildes[0], STDOUT_FILENO) == -1) {
                perror("oshell: _pipe() dup2 child");
                exit(EXIT_FAILURE);
            }
        default: // parent writes to pipe
            close(fildes[0]); // read end is unused
            // 'input' would write the second command ie grep xxx
            write(fildes[1], args[2], sizeof(args[2]));
            if(dup2(fildes[1], STDIN_FILENO) == -1) {
                perror("oshell: _pipe() dup2 parent");
                exit(EXIT_FAILURE);
            }
    }
    close(fildes[0]);
    close(fildes[1]);

    return 0;
}
