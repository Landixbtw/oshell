#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <complex.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>


#include "../../include/parse.h"
#include "../../include/utils.h"


int execute_command(char **args)
{
    char *command = strtok(args[0], " ");

    // Handle built-in args[0]
    if (strcmp(args[0], "exit") == 0)
    {
        fprintf(stderr,"Exiting Shell...\n");
        exit(EXIT_SUCCESS);
    }
    if (strcmp(args[0], "help") == 0)
    {
        show_usage();
        return 0;
    }

    assert(command != NULL);
    // FIX: These have to be desinged more compact this is basically the same code but with \n this is stupid

    // cd -> CD function - utils.c
    if (command != NULL && args[1] != NULL && strcmp("cd", command) == 0)
    {
        int status = change_directory(args[1]);
        if (status != 0) {
            perror("oshell: change_directory() error");
            return 1;
        }
        return 0;
    } else if (command != NULL && args[1] == NULL && strcmp("cd", command) == 0) {
        chdir(getenv("HOME"));
        return 0;
    }

    // kill -> kill function - utils.c
    if (args[0] != NULL && args[1] != NULL && strcmp("kill", command) == 0) {
        int status = kill_process(args[1]);
        if (status != 0) {
            perror("kill_by_name() Error");
            return 1;
        }
        return 0;
        // FIX: This does not work
    } else if (args[0] != NULL && args[1] == NULL && strcmp("kill", command) == 0) {
        fprintf(stderr,"Please give a process to kill.\n");
        return 1;
    }

    // pipe -> | function
    if (args[0] != NULL && args[1] != NULL && args[2] != NULL && strcmp("|", args[1]) == 0) {
        _pipe(args);
    }

    // clear -> clear function
    if (args[0] != NULL && strcmp("clear", command) == 0) {
        clear();
        return 0;
    }

    /*
     * input redirection -> < function
     * this function works directly with stdin, and changes the stdin, to the
     * file content the user wants to redirect. It basically duplicates the
     * file content into stdin, 'replacing' the old stdin,
     * */

    int saved_stdin = dup(STDIN_FILENO);
    int do_input_redirection = 0;
    int fd_in = 0;

    // FIX: This exits the shell, return 0 
    if (args[0] != NULL && args[1] != NULL && args[2] != NULL && strcmp("<", args[1]) == 0) {
        char *filename = args[2];
        FILE        *file = fopen(filename, "r");

        assert(file != NULL);

        if (feof(file) || ferror(file)) {
            perror("oshell: fread() failed");
            exit(EXIT_FAILURE);
        }

        // open filebased pipeline channel for file 'filename' in read only
        fd_in = open(filename, O_RDONLY);
        assert(fd_in != 0); // 0 = stdin | 1 = stdout | 2 = stderr

        close(STDIN_FILENO); // we close stdin
        // we duplicate fd, into stdin
        if (dup2(fd_in, STDIN_FILENO) == -1) {
            perror("oshell: dup2 error");
            close(fd_in);
            return 1;
        }
        args[1] = NULL; // remove '<' from the command

        do_input_redirection = 1;
    }

    /*
     *  output redirection
     *  This redirects the output from a command not to the console stream
     *  but to the file that the user specified
     *  https://stackoverflow.com/questions/12812579/how-redirection-internally-works-in-unix
     * */

    // we save the original stdout
    int saved_stdout = dup(STDOUT_FILENO);
    int do_redirection = 0;
    int fd = 0;

    // redirect stdout to the file
    if (args[0] != NULL && args[1] != NULL && args[2] != NULL &&
        (strcmp(">", args[1]) == 0 || strcmp(">>", args[1]) == 0)) {
        // > truncate (overwrite) ; >> append

        int flags = O_WRONLY | O_CREAT;
        if (strcmp(">>", args[1]) == 0)
            // inplace bitwise OR (x |= y ; x = x | y)
            // add flag O_APPEND to flags
            // &= ~xyz (remove xyz)
            flags |= O_APPEND;
        else
            flags |= O_TRUNC;

        fd = open(args[2], flags , 0644);
        assert(fd != -1);

        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("oshell: dup2 error");
            close(fd);
            return 1;
        }

        args[1] = NULL;
        args[2] = NULL;
        do_redirection = 1;
    }

    pid_t pid = 0;
    int status = 0;

    char *scmd = make_command(args);
    if ((pid = fork()) < 0)
        perror("oshell: fork() error");
    else if (pid == 0) {
        //child
        fprintf(stderr, "Command: %s %s %s \n", scmd, args[1], args[2]);
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
            sleep(1);
        } else {
            if (WIFEXITED(status)) {
                break;
                // printf("child exited with status of %d\n", WEXITSTATUS(status));
            } else {
                puts("child did not exit succesfully");
            }
        }
   } while (pid == 0);

    if (do_redirection) {
        fflush(stdout);
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
        if (fd != -1)
            close(fd);
    } else if (do_input_redirection) {
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
        if (fd_in != -1)
            close(fd_in);
    }
    return 0;
}
