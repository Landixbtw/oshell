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

    
    char ***commands = malloc((pipes_amount + 1) * sizeof(char**));
    char **paths = malloc((pipes_amount + 1) * sizeof(char*));

    fprintf(stderr, "after ***Commands");
    fflush(stderr);

    int start = 0;
    int end = 0;
    int cmd_idx = 0;
    for (; cmd_idx <= pipes_amount; cmd_idx++) {
        if(cmd_idx < pipes_amount) {
            end = pipe_pos[cmd_idx];
        } else {
            int i = 0;
            do {
                i++;
            }while (args[i] != NULL);
            end = i;
        }
    }
    fprintf(stderr, "after loop\n");
    fflush(stderr);

    // Create command array from start to end
    // commands[cmd_idx] = /* extract args[start] to args[end-1] */;
    
    for(int i = 0; i < end-1; i++) {
        commands[cmd_idx] = &args[i];
    }

    // Create path for this command
    size_t path_length = strlen("/usr/bin/") + strlen(commands[cmd_idx][0]) + 1;
    snprintf(paths[cmd_idx], path_length, "/usr/bin/%s", commands[cmd_idx][0]);
    
    start = end + 1; // Skip the pipe


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

    char **argv = NULL;

    int k = 0;
    while (commands[k + 1] != NULL) {
        k++;
    }

    argv = malloc((k + 1) * sizeof(char*));

    // k gives number of arguments
    for(int j = 0; j < k; j++) {
        argv[j] = *commands[j + 1];
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
    int n = pipes_amount + 1;

// echo "foo bar baz" | wc -w
// 3

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
    
    for(int i = 0; i < n; i++) {
        if (fork() == 0) {
            // child: setup the redirections
            if(i == 0) { // first command, only redirect stdout
                fprintf(stderr, "Child %d starting\n", i);
                if(dup2(fd[i][1], STDOUT_FILENO) == -1) {
                    return -1;
                }
                fflush(stdout);
                for(int j = 0; j < pipes_amount; j++) {
                    close(fd[j][0]);
                    close(fd[j][1]);
                }

            } else if (i == n - 1) { // last command only redirect stdin
                fprintf(stderr, "Child %d starting\n", i);
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
                fprintf(stderr, "Child %d starting\n", i);
                if(dup2(fd[i-1][0], STDIN_FILENO) == -1 || dup2(fd[i][1], STDOUT_FILENO) == -1) {
                    return -1;
                }
                for(int j = 0; j < pipes_amount; j++) {
                    close(fd[j][0]);
                    close(fd[j][1]);
                }
            }

            if (execv(*paths, argv) == -1) {
                perror("execv() failed");
                free(paths);
                free(argv);
                free(commands);
                close(fd[i][0]);
                close(fd[i][1]);
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

    for(int i = 0; i < pipes_amount; i++) {
        close(fd[i][0]);
        close(fd[i][1]);
    }

    // restore stdin/stdout
    dup2(saved_stdin, STDIN_FILENO);
    dup2(saved_stdout, STDOUT_FILENO);

    if(close(saved_stdin) != 0) perror("oshell: piping()");
    if(close(saved_stdout) != 0) perror("oshell: piping()");

    free(paths);
    free(argv);
    free(commands);

    return 0;
}
