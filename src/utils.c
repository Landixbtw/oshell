#include <stdio.h>
#include "../include/utils.h"
#include <unistd.h>
#include <stdlib.h>

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
    int ret;
    ret = chdir(directory);
    printf("changing to %s\n", directory);
    if (ret == -1) {
        perror("Could not change directory"); 
        return 1;
    } else {
        printf("succesfully changed directory to %s\n", directory);
        system("pwd");
    }
    // set errno for error x 
    return 0;
}

int kill_by_name(const char *process_name)
{
    printf("killed %s\n", process_name);
    // set errno for error x 
    return 0;
}
