/*
 *  input redirection 
 *  
 *  stdin redirection: < 
 *
 *  Redirect the content of a file to stdin of a command
 *
 *  command < file
 *
 *
 * */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../../include/parse.h"

char *input_redirection(const char *filename)
{
    // we want to open the file, read it, take out all whats in it. and execute that
    printf("Filename: %s", filename);
    FILE *file = fopen(filename, "r");
    assert(file);

    char *command = malloc(sizeof(&file));
    assert(command);
    fread(&command, sizeof(char), 1, file);

    printf("FILE CONTENT: %s", command);

    fclose(file);
    free(command);
    return command;
}
