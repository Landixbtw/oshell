#include <stdio.h>
#include <unistd.h>
#define _GNU_SOURCE

#include "../include/Header.h"

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
        char *proc_dir_name = "/proc/";
        DIR *dir = opendir(proc_dir_name);
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
        struct dirent *comm;

        /*
         * We need to open every /proc/pid/comm file  (opendir() and readdir()) and check if the content matches
         * the process name if yes, we have to corresponding pid and we can kill the 
         * process 
         * */
        // use opendir() to open proc/ and readdir() to go through /*/comm 
        // /proc/*/comm

        int count = 0;
        while((dirent = readdir(dir))!= NULL) {
            if(dirent->d_type == DT_DIR) {
                // we dont need /proc/. and /proc/..
                count++;
                if(strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0) {
                    continue;
                }

                if(!is_numeric(dirent->d_name)) continue;

                // read_proc_dir(proc_dir_name);
                FILE            *fp;
                size_t          ret;

                // string lenth for proc_dir_name + dirent->d_name
                size_t fullProcPathLength = strlen(proc_dir_name) + strlen(dirent->d_name) + 1;
                // invalid write size of 1 for snprintf
                // core dumped
                char *fullProcPath = malloc(fullProcPathLength);
                if(fullProcPath == NULL) {
                    perror("oshell: not able to allocate enough Memory for variable: 'fullProcPath'");
                    exit(EXIT_FAILURE);
                }
                snprintf(fullProcPath, fullProcPathLength ,"%s%s", proc_dir_name, dirent->d_name);
                if((pidDir = opendir(fullProcPath)) != NULL ) {

                    pidDirEnt = readdir(pidDir);

                    // this saves the dir name of the comm file we are about to visit
                    // we can "save" the dir name and convert + cast it to a pid so 
                    // that if the entry is the correct one we have access to the pid 
                    // and can kill the process via kill()
                    pid = (pid_t)atoi(dirent->d_name);

                    size_t commFilePathLength = strlen(proc_dir_name) + strlen(dirent->d_name) + strlen("/comm") + 1;
                    char *commFilePath = malloc(commFilePathLength);
                    if(commFilePath == NULL) {
                        perror("oshell: not able to allocate enough Memory for variable: 'commFilePath'");
                        exit(EXIT_FAILURE);
                    }
                    snprintf(commFilePath, commFilePathLength,"%s%s/comm", proc_dir_name, dirent->d_name);

                    fprintf(stderr, "\nfilepath for comm file: [%s]\n", commFilePath);

                    fp = fopen(commFilePath, "r");
                    if(!fp) {
                        perror("oshell: kill_process(): fopen()");
                        return EXIT_FAILURE;
                    } else {
                        fprintf(stderr, "opened file %s \n", commFilePath);
                    }
                    // https://stackoverflow.com/questions/4850241/how-many-bits-or-bytes-are-there-in-a-character
                    size_t          BUFFER_SIZE = sizeof(char);
                    long file_size = ftell(fp);

                    char   *buffer = malloc((BUFFER_SIZE * file_size) + 1);

                    // using sizeof(buffer) instead of BUFFER_SIZE for the 3rd 
                    // argument solve a weird edgecase, where the y for alacritty was cut off
                    ret = fread(buffer, sizeof(buffer), sizeof(buffer), fp);
                    fprintf(stderr, "file content: %s\n\n", buffer);

                    if(ferror(fp)) {
                        fprintf(stderr, "oshell: kill_process(): fread() failed: %zu\n", ret);
                        fclose(fp);
                        return -2;
                    }

                /*
                   we can check for the process name as a substring in the buffer 
                   the euser entered we want to save the pid and then kill the
                  process via kill(pid, 15)
                 */

                    // if one has ie \n this should be replaced with \0, but if some word ends with alpha char, then we have
                    // problem, might be the case?
                    if(strncmp(strip_non_alpha(buffer), strip_non_alpha(process_name_or_id), 5) == 0) {
                        fprintf(stderr, "match found: %s / %s\n", process_name_or_id, buffer);
                        kill(pid, 15);
                        fprintf(stderr,"killed %i\n", pid);
                        fclose(fp);
                        break;
                    } 
                    else if((strcmp(buffer, process_name_or_id) < 0) || (strcmp(buffer, process_name_or_id) > 0)) {
                        fprintf(stderr, "something is wrong, one is bigger then the other.\n");
                        fclose(fp);
                        break;
                    }
                }
            }
        }
        closedir(dir);
        fprintf(stderr, "Total entries found: %d\n", count);
        return 0;
    } else {
        pid_t pid = string_to_int(process_name_or_id);
        // NOTE: sig 15 nutzen statt 9?
        kill(pid, 15);
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

// claude.ai
int is_numeric(const char *str) {
    if (!str || *str == '\0') return 0;
    while (*str) {
        if (!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}


//One approach is to iterate through the string from the beginning 
//and end, and keep track of the first and last non-whitespace characters. 
//Then, you can create a new string with only the characters between these two positions.

char *strip_non_alpha(char *input_string) {
    fprintf(stderr, "string: %s \n" , input_string);
    for(int i = strlen(input_string) - 1; i >= 0; i--) {
        fprintf(stderr ,"Character: %c, ASCII: %d\n", input_string[i], input_string[i]);
        if(!isalpha(input_string[i])) {
            // remove i from string
            input_string[i] = '\0';
        } else {
            // add_NULL_terminator(input_string);
        }
    }
    return input_string;
}

char *add_NULL_terminator(char *string) {
    for(int i = strlen(string) - 1; i >= 0; i--) {
        if(isalpha(string[i])) {
            string[i + 1] = '\0';
        }
    } 
    return string;
}

