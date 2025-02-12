#ifndef PARSE_H_
#define PARSE_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdbool.h>

char **parse(char *input);
int execute_command(char **args);
char *input_redirection(const char *filename);

#endif
