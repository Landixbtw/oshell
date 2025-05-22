#include <signal.h>
#include <stdio.h>
#include "../include/utils.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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

/*
 * we convert the string to int
 * string: -1
 * int: the int could be 0?
 * error: -2 ... 
 * */
int string_to_int(char *string) {
    long val = 0;
    int base = 10;
    char *endptr, *str;
    str = string;
    errno = 0;
    val = strtol(str, &endptr, base);

    // check for errors
    if (errno == ERANGE) {
        perror("strtol");
        return -2;
    }

    if (endptr == str) {
        return -1;
    }
    return val;
}

// https://www.man7.org/linux/man-pages/man2/getpid.2.html
/*
 * getpid only gets the pid from the calling process, 
 * */
int kill_process(char *process_name_or_id)
{
    // kill(2)
    // kill(__pid_t pid, int sig)
    // need to check if input is pid or process name
    // need to get the pid from the name
    // pass the pid to the kill function, with signal for kill (is 9)?

    // c23 has typeof, c11 does not how can we check if process_name_or_id
    // is int or char*
    //
    // if string_to_int returns 1 this mean there were no numbers found
    if(string_to_int(process_name_or_id) == -1) {
        /*
         * We need to open every /proc/pid/cmdline file  (opendir() and readdir()) and check if the content matches
         * the process name if yes, we have to corresponding pid and we can kill the 
         * process 
         * */

        pid_t pid = 0;
        // process_name_or_id is string
        kill(pid, 9);
        fprintf(stderr,"killed %i\n", pid);
        return 0;
    } else {
        pid_t pid = string_to_int(process_name_or_id);
        kill(pid, 9);
        fprintf(stderr,"killed %i\n", pid);
        return 0;
    }
    return -1;
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

