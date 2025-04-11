#include <stdio.h>
#include "../include/utils.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../include/parse.h"

void show_usage()
{
    fprintf(stderr, "oshell usage: \n"
        "\tthis is just a basic shell, I made for fun.\n"
        "\n\texit: will exit the shell\n"
        "\thelp: will print this menu\n"
    "\n");
}

int change_directory(const char *directory)
{
    fprintf(stderr, "changing to %s\n", directory);
    int ret;
    ret = chdir(directory);
    if (ret == -1) {
        return 1;
    } else {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        fprintf(stderr,"%s", cwd);
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
    fprintf(stderr,"killed %s\n", process_name_or_id);
    // set errno for error x
    return 0;
}


// this reads the input
char *oshell_read_line()
{
    char *line = NULL;
    size_t bufsize = 0;

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

    execute_command(args);
    free(args);
}

char *make_command(char **args) {
    size_t scmd_len = strlen("/usr/bin/") + strlen(args[0]) + 1;
    char *scmd = malloc(scmd_len);
    if(scmd == NULL) {
        perror("oshell: not able to allocate enough Memory for scmd");
        exit(EXIT_FAILURE);
    }
    snprintf(scmd, scmd_len ,"/usr/bin/%s", args[0]);
    fprintf(stderr, "%s\n", scmd);
    return scmd;
}
