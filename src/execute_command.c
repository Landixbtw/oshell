#include "../include/Header.h"
#include <string.h>
#include <errno.h>

/*
 * NOTE: More explanation on pointer to pointer, why? what? how?
 * char ** is a pointer to a pointer
 * */

// claude.ai
int command_exists(const char *command) {
    // For absolute/relative paths
    if (strchr(command, '/')) {
        return access(command, X_OK) == 0;
    }
    
    // Search PATH
    char *path_env = getenv("PATH");
    if (!path_env) return 0;
    
    char *path = strdup(path_env);
    char *dir = strtok(path, ":");
    
    while (dir) {
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);
        
        if (access(full_path, X_OK) == 0) {
            free(path);
            return 1;  // Found it
        }
        dir = strtok(NULL, ":");
    }
    
    free(path);
    return 0;  // Not found
}

int execute_command(char **args)
{
    // args[0] should always be the main command
    char *command = strtok(args[0], " ");

    // if the command is NULL meaning just enter, we just return
    if (command == NULL) return 0;


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
    if (strcmp("cd", command) == 0)
    {
        if(args[1] != NULL) {
            int status = change_directory(args[1]);
            if (status != 0) {
                fprintf(stderr, "change_directory() error: %s \nTo avoid this error make sure this directory you want to navigate to exists. \n\t directory and Directory are not the same.\n", strerror(errno));
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
        if(pipe_redirection(args) != 0) {
            // perror("oshell: pipe_redirection()");
        }
        // this is needed otherwise, the code will try to execute the command provided e.g.
        // echo "foo bar baz" | wc -w just like that. 
        // this would output foo bar baz | wc -w 
        return 0;
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
        // < does not check if the file in the argument is a valid file, it will redirect anything
        char        *filename = args[2];
        FILE        *file = fopen(filename, "r");

        if(file == NULL) {
            fprintf(stderr, "Error: %s is not a valid file.", filename);
        }

        if (feof(file) || ferror(file)) {
            perror("oshell: fread() failed");
            exit(EXIT_FAILURE);
        }

        // open filebased pipeline channel for file 'filename' in read only
        // 0 = stdin | 1 = stdout | 2 = stderr

        fd_in = open(filename, O_RDONLY);

        close(STDIN_FILENO); // we close stdin
        // we duplicate fd, into stdin
        if (dup2(fd_in, STDIN_FILENO) == -1) {
            perror("oshell: dup2 error");
            close(fd_in);
            return -1;
        }
        args[input_red_pos] = NULL; // remove '<' from the command

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
    int do_output_redirection = 0;
    int fd = 0;

    int truncate_pos = find_shell_operator(">", args);
    int append_pos = find_shell_operator(">>", args);
    // redirect stdout to the file
    if ((append_pos > 0 && args[append_pos + 1] != NULL) || (truncate_pos > 0 && args[truncate_pos + 1] != NULL)) {
        // > truncate (overwrite) ; >> append
        
        if (args[truncate_pos + 1] == NULL || args[append_pos + 1] == NULL)  {
            fprintf(stderr, "Error: missing filename after > / >> \n");
            return -1;
        }

        if (args[truncate_pos + 2] != NULL || args[append_pos + 2] != NULL) {
            fprintf(stderr, "Error: too many arguments after > / >> \n");
            return -1;
        }

        char *filename = NULL;
        // if (append_pos != -1) { append_pos } else truncate_pos
        int pos = (append_pos != -1) ? append_pos : truncate_pos;
        if (pos != -1) {
            filename = args[pos + 1];
        }

        int flags;
        if (strcmp(">>", (args[append_pos])) == 0) {
            // inplace bitwise OR (x |= y ; x = x | y)
            // add flag O_APPEND to flags
            // &= ~xyz (remove xyz)
            flags = O_APPEND | O_WRONLY | O_CREAT; // create if needed, append if exists
        } else {
            flags = O_TRUNC | O_WRONLY | O_CREAT; // create if needed, truncate if exists
        }

        fd = open(filename, flags, 0644);
        if(fd == -1) perror("oshell: open() failed");

        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("oshell: dup2 error");
            close(fd);
            return 1;
        }

        // this is to remove the operator and filename from the command
        args[pos] = NULL;
        args[pos + 1] = NULL;

        do_output_redirection = 1;
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
    if(dollar_pos >= 0 && args[dollar_pos][1]) {
        char envChar = args[dollar_pos][0];
        // we start at the first second char, this should be the first letter after
        // $ and we copy everything short of 1 and put it together into one String
        strncpy(envVar, &args[dollar_pos][1], sizeof(args[dollar_pos] -1));

        char envCharStr[2] = {envChar, '\0'};  // Convert to a proper string
        // when we just use envChar, (a non null terminated char) strcmp wont now when the
        // strings ends, and just read garbage
        if(strcmp("$",envCharStr) == 0) {
            // fprintf(stderr, "passing %s to secure_getenv\n", envVar);
            fprintf(stderr, "%s \n\n", secure_getenv(envVar));
            free(envVar);
            return 0;
        }
    }


    pid_t pid = 0;
    int status = 0;

    /*
     * For the shell to work, proper we want to identify what is a command, and what is giberish
     * if the user enters a, that should return an error with "oshell: command not found: [command user entered]"
     * because otherwise it will try to execute the command a and this will obv not work if a is not a valid command for the 
     * shell or in /usr/bin
     *
     * BRAINSTROM
     *
     * Looping through everything in /usr/bin is just dumb this will take way too long no? For me its about 3800 entries so thats not happening
     *
     * just rule out anything that is shorter then strlen(x)? 
     *
     * --> command_exists with access()
     * */


    char *new_command = make_command(args);
    if(!command_exists(new_command)) {
        fprintf(stderr, "execute_command(): command '%s' not found\n", args[0]);
        return -1;
    }
    if ((pid = fork()) < 0)
        perror("oshell: fork() error");
    else if (pid == 0) {
        //child
        //fprintf(stderr, "Command: %s %s %s \n", new_command, args[1], args[2]);
        int res = execv(new_command, args);
        if (res == -1) {
            fprintf(stderr, "freeing new_command at %p in function execute_command\n", (void*)new_command);
            free(new_command);
            new_command = NULL;
            exit(EXIT_FAILURE);
        } else {
            fprintf(stderr, "freeing new_command at %p in function execute_command\n", (void*)new_command);
            free(new_command);
            new_command = NULL;
            sleep(5);
            return(1);
        }
    }
    else do {
        if ((pid = waitpid(pid, &status, WNOHANG)) == -1)
            perror("oshell: waitpid() error");
        else if (pid == 0) {
            sleep(1);
        } else {
            if (WIFEXITED(status)) {
                free(new_command);
                break;
                // printf("child exited with status of %d\n", WEXITSTATUS(status));
            } else {
                puts("child did not exit successfully");
                free(new_command);
            }
        }
   } while (pid == 0);

    // this needs to be here, after the commands have been execute ie < > >> otherwise it will just try and pass an empty stdin stdout
    if (do_output_redirection) {
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


