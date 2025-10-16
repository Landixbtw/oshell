#include "../include/Header.h"

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>

int main(void){
    // TODO: This takes a long as time for the shell to then be responsive is
    // there a way to make it shorter ?

    struct termios term;

    if(isatty(STDIN_FILENO) == 1) {
        /*
         * This code before the for loop enables "canonical mode" meaning
         * that something like the backspace/delete key is properly handled by
         * the terminal AFAIk
         * */

        // get current terminal attributes
        if (tcgetattr(STDIN_FILENO, &term) == -1) {
            perror("tcgetattr");
            exit(EXIT_FAILURE);
        }

        // turn on canonical mode and echo
        term.c_lflag |= ICANON | ECHOE | ECHOK;

        // apply changes immediately
        if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1) {
            perror("tcsetattr");
            exit(EXIT_FAILURE);
        }
    } else {
        // Indicates that "oshell" is not opened by terminal but other program, like 
        // golang for testing
        cfmakeraw(&term);
        if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1) {
            perror("tcsetattr");
            exit(EXIT_FAILURE);
        }
    }

    for(;;) {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        // we print all the "constant" stuff, to stderr, because if you use
        // > >> the stdout is sent to the file, so this will also be sent to the file
        // so all shell prompts messages etc that is not from the user is printed to stderr
        char* user = getenv("USER");
        char *hostname = malloc(128);
        assert(hostname != NULL);
        if(gethostname(hostname, 128) == -1) {
            perror("oshell: gethostname() error");
            exit(EXIT_FAILURE);
        }

        // for example
        // ole@tux
        // /home / ole / Dokumente / Projekte / c / oshell

        fprintf(stderr, "%s@%s\n", user, hostname);
        fprintf(stderr, " %s > \n", cwd);
        fflush(stderr);

        oshell_loop();
    }
    return 0;
}
