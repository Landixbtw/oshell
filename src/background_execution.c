#include "Header.h"

int background_execute(char **process) {

    /*
     * We have to take the process, and prop fork?
     *  https://stackoverflow.com/questions/12824848/placing-a-process-in-the-background-in-c
     *
     * */

    pid_t pid = 0;
    int status = 0;

    // do we need to make a command?
    // as far as I remember this retunrns /usr/bin/process, 
    char *command = make_command(process); // WARN: FREE
    if ((pid = fork()) < 0) {
        perror("fork");
    } else if (pid == 0) {
        // this is the child,

        // what all does execv need
        int res = execv(command);
        if (res == -1) {
            perror("execv");
            free(command);
            return(1);
        }
        free(command);
        return 1;
    } else do {
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

    return 0;
}
