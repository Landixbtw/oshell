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
    char **cmd2 = &args[pipe_pos + 1 ];

    int fildes[2];
    const int BSIZE = 100;
    char buf[BSIZE];
    ssize_t nbytes = 0;


    // NOTE: assert only for debugging, since it disapears in the "release" build
    if (pipe(fildes) == -1) {
        perror("ohsell: _pipe");
        return -1;
    }

    fcntl(fildes[0], F_SETFD, FD_CLOEXEC); // read end 
    fcntl(fildes[1], F_SETFD, FD_CLOEXEC); // write end 

    char *scmd = "";

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
            if (read(fildes[0], args[0], sizeof(args[0])) > 0)
                // 'buf' reads, the first command ie ls (ls | grep xxx)
                puts("oshell: _pipe(): EOF or error detected.");
            if(dup2(fildes[0], STDOUT_FILENO) == -1) {
                perror("oshell: _pipe() dup2");
                exit(EXIT_FAILURE);
            }
	    fprintf(stderr, "cmd1: %s\n", cmd1[0]);
	    fprintf(stderr, "cmd2: %s\n", cmd2[0]);
            scmd = make_command(cmd1);
	    // FIX: command one and two are added to scmd, usr/bin/lsgrep
            fprintf(stderr, "scmd: %s", scmd);
            // how can "|" be added as arg ? rn command is lsgrep not ls | grep xxx
            execv(scmd, cmd2);
        default: // parent writes to pipe
            close(fildes[0]); // read end is unused
            // 'input' would write the second command ie grep xxx
            write(fildes[1], args[2], sizeof(args[2]));
            if(dup2(fildes[1], STDIN_FILENO) == -1) {
                perror("oshell: _pipe() dup2");
                exit(EXIT_FAILURE);
            }
            // scmd = make_command(cmd1);
            // execv(scmd, cmd2);
    }
    free(scmd);
    close(fildes[0]);
    close(fildes[1]);

    return 0;
}
