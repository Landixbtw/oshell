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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>

char **_pipe(char **args)
{
    int fildes[2];
    const int BSIZE = 100;
    char buf[BSIZE];
    ssize_t nbytes = 0;
    int status = 0;

    status = pipe(fildes);
    assert(status != -1);

    fcntl(fildes[0], F_SETFD, fcntl(fildes[0], F_GETFD | FD_CLOEXEC)); // read end
    fcntl(fildes[1], F_SETFD, fcntl(fildes[1], F_GETFD | FD_CLOEXEC)); // write end

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
            dup2(fildes[0], STDOUT_FILENO);
            fprintf(stderr, "fildes[0] %i\n", fildes[0]);
            // execvp
            close(fildes[0]);
            close(fildes[1]);
            exit(EXIT_SUCCESS);

        default: // parent writes to pipe
            close(fildes[0]); // read end is unused
            // 'input' would write the second command ie grep xxx
            write(fildes[1], args[2], sizeof(args[2]));
            dup2(fildes[1], STDIN_FILENO);
            fprintf(stderr, "fildes[1] %i\n", fildes[1]);
            // execvp
            close(fildes[0]);
            close(fildes[1]);
            exit(EXIT_SUCCESS);
    }
    return 0;
}
