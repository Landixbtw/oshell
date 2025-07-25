#define GNU_SOURCE

#include "../include/Header.h"

/*
 * NOTE: More explanation on pointer to pointer, why? what? how?
 * char ** is a pointer to a pointer
 * */

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
    // cd -> CD function - utils.c
    if (args[1] != NULL && strcmp("cd", command) == 0)
    {
        int status = change_directory(args[1]);
        if (status != 0) {
            perror("oshell: change_directory() error");
            return 1;
        }
        return 0;
    }
    if (args[1] == NULL && strcmp("cd", command) == 0) {
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
    }
    if (args[0] != NULL && args[1] == NULL && strcmp("kill", command) == 0) {
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
        //fprintf(stderr, "Command: %s %s %s \n", scmd, args[1], args[2]);
        int res = execv(scmd, args);
        if (res == -1) {
            perror("execv() failed");
            free(scmd);
            exit(EXIT_FAILURE);
        }
        free(scmd);
        sleep(5);
        return(1);
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
                puts("child did not exit successfully");
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
    // env -> getenvvar
    // if the first char after echo is a $ we want to pass it on to getenvvar()
    // else just pass it to execute
    // init envVar
    char *envVar = malloc(sizeof(args[1]));
    if(envVar == NULL) {
        fprintf(stderr, "execute_command(): *envVar: Error when trying to allocate 'sizeof(%s)' memory. \n", args[1]);
        return -1;
    }
    // NOTE: echo $HOST, echo $OSTYPE just parrot back, on oshell, but return something valid with zsh
    if(args[1]) {
        char envChar = args[1][0];
        // we start at the first second char, this should be the first letter after
        // $ and we copy everything short of 1 and put it together into one String
        strncpy(envVar, &args[1][1], sizeof(args[1] -1));

        char envCharStr[2] = {envChar, '\0'};  // Convert to a proper string
        // when we just use envChar, (a non null terminated char) strcmp wont now when the
        // strings ends, and just read garbage strcmp wont now when the
        // strings ends, and just read garbage
        if(args[0] != NULL && args[1] != NULL && strcmp("$",envCharStr) == 0) {
            secure_getenv(envVar);
            free(envVar);
            return 0;
        }
    }
    return 0;
}


