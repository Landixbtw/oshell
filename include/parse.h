#ifndef PARSE_H_
#define PARSE_H_

char **parse(char *input);
bool is_valid_command(const char *command);
void execute_command(char *input, char **args);

#endif
