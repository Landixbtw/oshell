// TODO: UNTANGLE THIS WHOLE MESS

#ifndef HEADER_H_
#define HEADER_H_

#define _GNU_SOURCE     // needed for ie gethostname()
#define _DEFAULT_SOURCE // needed for ie DT_DIR utils ~ 102

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include <assert.h>
#include <complex.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

// parse.h
char **parse(char *input);
int execute_command(char **args);

// needed for utils.h
void show_usage(void);

void help(void);
int change_directory(char *directory);
int kill_process(char *process_name_or_id);
void oshell_loop(void);
void clear(void);
int pipe_redirection(char **args);
char *make_command(char **args);
int is_numeric(const char *str);
char *strip_non_alpha(char *input_string);
char *add_NULL_terminator(char *string);
int string_to_int(char *string);

int input_redirection(char **args);

int find_shell_operator(char *operator, char ** args);

#endif
