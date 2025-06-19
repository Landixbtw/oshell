#include "../include/Header.h"
#include <stdio.h>

/*
*Sure! Here are the main issues I identified in your original code:

## Primary Issues:

✅ 1. **Premature loop termination**: You have `break;` statements in both the success and failure cases that cause the function to exit the loop after checking only 1-2 processes instead of continuing through all `/proc/PID/` directories.

✅ 2. **File size calculation problem**: You're using `ftell(fp)` immediately after opening the file, but the file pointer is at position 0, so `ftell()` returns 0. This means you're allocating almost no buffer space.

✅ 3. **Buffer reading issues**: The `fread()` approach with `sizeof(buffer)` as parameters is problematic when `buffer` is a pointer.

4. **Newline handling**: The `/proc/PID/comm` files contain a trailing newline character that needs to be stripped for accurate string comparison.

5. **Logic error**: The `else if((strcmp(buffer, process_name_or_id) < 0) || (strcmp(buffer, process_name_or_id) > 0))` condition will always be true for non-matching strings, causing an immediate break.

6. **Memory leaks**: `buffer` and `comm_file_path` aren't freed in all code paths.

## Secondary Issues:

7. **Inconsistent error handling**: Some error paths don't properly clean up allocated memory before exiting.

8. **Debugging confusion**: The current debug output makes it hard to see what's actually being compared due to the newline issue.

The core problem is that your algorithm is sound, but the early termination and file reading issues prevent it from working consistently. Fix those and it should work reliably!
 *
 */


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
            //perror("oshell: failed to open '/proc/'");
            exit(EXIT_FAILURE);
        }
        // if(dir == EACCES || ENOMEM) {
        //     perror("oshell: ");
        //     exit(EXIT_FAILURE);
        // }

        char   *buffer;
        char *comm_file_path;
        char *full_proc_path;

        FILE            *fp;

        bool process_found = false;
        bool cleanup = true;

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
                //
                // string length for proc_dir_name + dirent->d_name
                size_t full_proc_pathLength = strlen(proc_dir_name) + strlen(dirent->d_name) + 1;
                // invalid write size of 1 for snprintf
                // core dumped
                full_proc_path = malloc(full_proc_pathLength);
                if(full_proc_path == NULL) {
                    //perror("oshell: not able to allocate enough Memory for variable: 'full_proc_path'");
                    exit(EXIT_FAILURE);
                }
                snprintf(full_proc_path, full_proc_pathLength ,"%s%s", proc_dir_name, dirent->d_name);
                if((opendir(full_proc_path)) != NULL ) {

                    // this saves the dir name of the comm file we are about to visit
                    // we can "save" the dir name and convert + cast it to a pid so
                    // that if the entry is the correct one we have access to the pid
                    // and can kill the process via kill()
                    pid = (pid_t)atoi(dirent->d_name);

                    size_t comm_file_pathLength = strlen(proc_dir_name) + strlen(dirent->d_name) + strlen("/comm") + 1;
                    comm_file_path = malloc(comm_file_pathLength);
                    if(comm_file_path == NULL) {
                        //perror("oshell: not able to allocate enough Memory for variable: 'comm_file_path'");
                        free(full_proc_path);
                        free(comm_file_path);
                        exit(EXIT_FAILURE);
                    }
                    snprintf(comm_file_path, comm_file_pathLength,"%s%s/comm", proc_dir_name, dirent->d_name);

                    fp = fopen(comm_file_path, "r");
                    if(fp == NULL) {
                        //perror("oshell: kill_process(): fopen()");
                        free(comm_file_path);
                        free(full_proc_path);
                        // no fclose here, because if fp is NULL we cant close a file 
                        // that is NULL
                        return EXIT_FAILURE;
                    }

                    // there is something with the /proc/*/comm files that causes issues with
                    // using ftell. so this is seems to be a way to do it.

                    long filesize = 0;
                    while (getc(fp) != EOF)
                        filesize++;
                    fseek(fp, 0, SEEK_SET);

                    // https://stackoverflow.com/questions/4850241/how-many-bits-or-bytes-are-there-in-a-character
                    size_t          BUFFER_SIZE = ((sizeof(char) * filesize) +1);

                    buffer = malloc(BUFFER_SIZE);

                    fgets(buffer, BUFFER_SIZE, fp);

                    if(ferror(fp)) {
                        //perror("kill_process(): fgets failed ");
                        free(buffer);
                        free(comm_file_path);
                        free(full_proc_path);
                        fclose(fp);
                        return -2;
                    }

                    // TODO: When trying to use kill a second time we get error:
                    // kill_by_name() Error: Too many open files
                    // why could that be?

                    if(strncmp(buffer, process_name_or_id, 5) == 0) {
                        // fprintf(stderr, "match found: %s / %s\n", process_name_or_id, buffer);
                        kill(pid, 15);
                        fprintf(stderr,"killed %i\n", pid);
                        process_found = true;
                        cleanup = false;
                        free(buffer);
                        free(comm_file_path);
                        free(full_proc_path);
                        fclose(fp);
                        break;
                    } 
                }
            }
        }
        if(!process_found) {
            fprintf(stderr, "Could not find process '%s'\n", process_name_or_id);
        } 
        if (cleanup) {
            fprintf(stderr, "cleaning up...\n");
            free(buffer);
            free(comm_file_path);
            free(full_proc_path);
            fclose(fp);
        }
        closedir(dir);
        fprintf(stderr, "Total entries found: %d\n", count);
        return 0;
    } else {
        pid_t pid = string_to_int(process_name_or_id);
        kill(pid, 15);
        fprintf(stderr,"killed %i\n", pid);
        return 0;
    }
    return -1;
}
