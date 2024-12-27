#ifndef PARSE_H_
#define PARSE_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdbool.h>



char **parse(char *input);
bool is_valid_command(const char *command);
void execute_command(char *input, char **args);

#endif
