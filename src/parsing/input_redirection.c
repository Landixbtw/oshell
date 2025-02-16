#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


#include "../../include/parse.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void input_redirection(const char *filename)
{
    // we want to open the file, read it, take out all whats in it. and execute that
    FILE        *file = fopen(filename, "r");
    size_t      SIZE = 512;
    char        buffer[SIZE];
    char        *command = malloc(SIZE);

    assert(file != NULL);
    assert(command != NULL);

    // https://cs50.harvard.edu/x/2025/shorts/file_pointers/

    if (feof(file) || ferror(file)) {
        perror("oshell: fread() failed");
        exit(EXIT_FAILURE);
    }

    /*
     * TODO: 
     * iterate through the buffer, and remove every \n before passing it to the
     * command, that way something like sort can properly deal with it
     * */

    // NOTE: this is not needed, and absolutely useless, will keep maybe I need it

    // int ret = 0;
    // while((ret = fread(buffer, sizeof(*buffer), ARRAY_SIZE(buffer), file)) != 0)
    // {
    //     size_t i = 0;
    //     size_t cmd_pos = 0;
    //     do {
    //         if (buffer[i] == '\n') { 
    //             buffer[i] += ' ';
    //         }
    //         printf("buffer[%ld]: %c ", i ,buffer[i]);
    //         // strcat requires a null terminated string
    //             if(i == ret) {
    //                 buffer[strlen(buffer) - 1] = '\0';
    //                 printf("buffer[%ld]: %c ", i ,buffer[i]);
    //                 strcat(command, &buffer[i]);
    //                 break;
    //             }
    //
    //         command[cmd_pos++] = buffer[i];
    //         buffer[i++] += ' ';
    //     i++;
    //     }while (i < strlen(buffer));
    // }
}
