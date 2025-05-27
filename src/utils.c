#include <cerrno>
#include <signal.h>
#include <stdio.h>
#include "../include/utils.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>

#include "../include/parse.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

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

    // if string_to_int returns 1 this mean there were no numbers found
    if(string_to_int(process_name_or_id) == -1) {
        struct dirent *dirent;
        pid_t pid = 0;
        char *procDirName = "/proc/";
        DIR *dir = opendir(procDirName);
        if(dir == NULL) {
            perror("oshell: failed to open '/proc/'");
            exit(EXIT_FAILURE);
        }
        // if(dir == EACCES || ENOMEM) {
        //     perror("oshell: ");
        //     exit(EXIT_FAILURE);
        // }

        DIR *pidDir;
        struct dirent *pidDirEnt;
        struct dirent *cmdline;

        /*
         * We need to open every /proc/pid/cmdline file  (opendir() and readdir()) and check if the content matches
         * the process name if yes, we have to corresponding pid and we can kill the 
         * process 
         * */
        // use opendir() to open proc/ and readdir() to go through /*/cmdline 
        // /proc/*/cmdline


        while((dirent = readdir(dir))!= NULL) {
            if(dirent->d_type == DT_DIR) {
                // we dont need /proc/. and /proc/..
                // NOTE: for somereason dirent->d_name is always .
                if(strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0) {
                    continue;
                }
                FILE            *fp;
                size_t          ret;
                unsigned char   buffer[4];

                // string lenth for procDirName + dirent->d_name
                size_t fullProcPathLength = strlen(procDirName) + strlen(dirent->d_name) + 1;
                // invalid write size of 1 for snprintf
                // core dumped
                char *fullProcPath = malloc(fullProcPathLength);
                if(fullProcPath == NULL) {
                    perror("oshell: not able to allocate enough Memory for variable: 'fullProcPath'");
                    exit(EXIT_FAILURE);
                }
                snprintf(fullProcPath, fullProcPathLength ,"%s%s", procDirName, dirent->d_name);
                fprintf(stderr, "dirent->d_name: %s\n", dirent->d_name);
                if((pidDir = opendir(fullProcPath)) != NULL ) {

                    pidDirEnt = readdir(pidDir);
                    size_t cmdlineFilePathLength = strlen(procDirName) + strlen(dirent->d_name) + strlen("/cmdline") + 1;
                    char *cmdLineFilePath = malloc(cmdlineFilePathLength);
                    if(cmdLineFilePath == NULL) {
                        perror("oshell: not able to allocate enough Memory for variable: 'cmdLineFilePath'");
                        exit(EXIT_FAILURE);
                    }
                    snprintf(cmdLineFilePath, cmdlineFilePathLength,"%s%s/cmdline", procDirName, dirent->d_name);

                    fprintf(stderr, "filepath: [%s]\n", cmdLineFilePath);

                    fp = fopen(cmdLineFilePath, "r");
                        if(!fp) {
                            perror("oshell: kill_process(): fopen()");
                            return EXIT_FAILURE;
                        }
                    ret = fread(buffer, sizeof(*buffer) , ARRAY_SIZE(buffer), fp);
                    if(ret != ARRAY_SIZE(buffer)) {
                        fprintf(stderr, "oshell: kill_process(): fread() failed: %zu\n", ret);
                        exit(EXIT_FAILURE);
                    }
                    // if(process_name_or_id == '\0' ) {
                        fprintf(stderr, "file content: %s", buffer);
                    // }
                    fclose(fp);
                }
            }
        // process_name_or_id is string
        // NOTE: sig 15 nutzen statt 9?
        // kill(pid, 9);
        // fprintf(stderr,"killed %i\n", pid);
        closedir(dir);
        return 0;
        }
    } else {
        pid_t pid = string_to_int(process_name_or_id);
        // NOTE: sig 15 nutzen statt 9?
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

