#include <stdio.h>
#include "../include/utils.h"
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include "../include/parse.h"

void show_usage()
{
    printf("oshell usage: \n"
        "\t this is just a basic shell, just not as feature rich as zsh or bash.\n"
        "\nexit: will exit the shell\n"
        "help: will print this menu\n"
    "\n");
}

int change_directory(const char *directory)
{
    printf("changing to %s\n", directory);
    int ret;
    ret = chdir(directory);
    if (ret == -1) {
        return 1;
    } else {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        printf("succesfully changed directory to %s\n", directory);
        printf("%s", cwd);
    }
    // set errno for error x 
    return 0;
}

int kill_process(const char *process_name_or_id)
{
    // kill(2)
    // kill(__pid_t pid, int sig)
    // need to check if input is pid or process name
    // need to get the pid from the name
    // pass the pid to the kill function, with signal for kill (is 9)?
    printf("killed %s\n", process_name_or_id);
    // set errno for error x 
    return 0;
}


// this reads the input
char *oshell_read_line()
{
    char *line = NULL;
    ssize_t bufsize = 0;

    if(getline(&line, &bufsize, stdin) == -1)
    {
        if(feof(stdin))
        {
            // EOF
            exit(EXIT_SUCCESS);
        } else {
            perror("oshell: readline");
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

void oshell_loop()
{
    char *user_input = oshell_read_line();
    char **args = parse(user_input);

    // TODO: what to do if command not valid system command? 
    // if (is_valid_command(args) == false) exit(EXIT_FAILURE);
    execute_command(args);
    free(args);
}

