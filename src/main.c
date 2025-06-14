#include "../include/Header.h"

int main(void){
    for(;;) {
        // TODO: infront of the wd should be user@machine e.g ole@tux
        //
        // TODO: This gets redirected to a file if the user does output
        // redirection and then the console has no prompt
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

        fprintf(stderr ,"%s@%s\n", user, hostname);
        fprintf(stderr, " %s > ", cwd);
        fflush(stderr);

        oshell_loop();
    }
    return 0;
}
