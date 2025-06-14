#pragma once

#ifndef HEADER_H_
#define HEADER_H_

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdbool.h>

#include <sys/ioctl.h>

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <assert.h>
#include <fcntl.h>

#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>


// gives you the length of an array
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// parse.h
char **parse(char *input);
int execute_command(char **args);

// needed for utils.h
void show_usage(void);

void help(void);
int change_directory(const char *directory);
int kill_process(char *process_name_or_id);
void oshell_loop();
void clear(void);
int _pipe(char **args);
char *make_command(char **args);
int is_numeric(const char *str);
char *strip_non_alpha(char *input_string);
char *add_NULL_terminator(char *string);




#endif
