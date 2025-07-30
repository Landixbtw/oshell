#define GNU_SOURCE

#include "../include/Header.h"

/*
 * NOTE: More explanation on pointer to pointer, why? what? how?
 * char ** is a pointer to a pointer
 * */

int execute_command(char **args)
{
    // args[0] should always be the main command
    char *command = strtok(args[0], " ");

    // if the command is NULL meaning just enter, we just return
    if (command == NULL) return 0;

    // Handle built-in args[0] --> isnt this just command?
    if (strcmp(command, "exit") == 0)
    {
        fprintf(stderr,"Exiting Shell...\n");
        exit(EXIT_SUCCESS);
    }
    if (strcmp(args[0], "help") == 0)
    {
        show_usage();
        return 0;
    }

    // cd -> CD function - utils.c
    // FIX: ??????
    if (strcmp("cd", command) == 0)
    {
        if(args[1] != NULL) {
            int status = change_directory(args[1]);
            if (status != 0) {
                perror("oshell: change_directory() error");
                return 1;
            }
        } else {
            chdir(getenv("HOME"));
        }
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
    int pipe_pos = find_shell_operator("|", args);
    if (pipe_pos > 0 && args[pipe_pos + 1] != NULL) {
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

    int input_red_pos = find_shell_operator("<", args);

    if (input_red_pos > 0 && args[input_red_pos + 1] != NULL) {
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

    // NOTE: Need to find a better way to check if there is ie > >> this right now only works if >> is the second "command"

    int output_red_pos = find_shell_operator(">", args);
    int append_pos = find_shell_operator(">", args);
    // redirect stdout to the file
    if ((append_pos > 0 && args[append_pos + 1] != NULL) || (output_red_pos > 0 && args[output_red_pos + 1] != NULL)) {
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


 // env -> getenvvar
    // if the first char after echo is a $ we want to pass it on to getenvvar()
    // else just pass it to execute
    // init envVar
    char *envVar = malloc(sizeof(args[1]));
    if(envVar == NULL) {
        fprintf(stderr, "execute_command(): *envVar: Error when trying to allocate 'sizeof(%s)' memory. \n", args[1]);
        return -1;
    }

    int dollar_pos = find_shell_operator("$", args);
    if(dollar_pos >= 0 && args[dollar_pos + 1]) {
        char envChar = args[dollar_pos][0];
        // we start at the first second char, this should be the first letter after
        // $ and we copy everything short of 1 and put it together into one String
        //
        // NOTE: needs to be adjusted, to use dollar_pos
        strncpy(envVar, & args[dollar_pos][1], sizeof(args[dollar_pos] -1));

        char envCharStr[2] = {envChar, '\0'};  // Convert to a proper string
        // when we just use envChar, (a non null terminated char) strcmp wont now when the
        // strings ends, and just read garbage
        if(strcmp("$",envCharStr) == 0) {
            // fprintf(stderr, "passing %s to secure_getenv\n", envVar);
            fprintf(stderr, "%s", secure_getenv(envVar));
            free(envVar);
            return 0;
        }
    }


    pid_t pid = 0;
    int status = 0;

    char *new_command = make_command(args);
    if ((pid = fork()) < 0)
        perror("oshell: fork() error");
    else if (pid == 0) {
        //child
        //fprintf(stderr, "Command: %s %s %s \n", new_command, args[1], args[2]);
        int res = execv(new_command, args);
        if (res == -1) {
            perror("execv() failed");
            free(new_command);
            exit(EXIT_FAILURE);
        }
        free(new_command);
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
    return 0;
}


