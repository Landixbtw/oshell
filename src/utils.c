#include "../include/Header.h"
#include "../include/chaining.h"
#include "../include/parsing_utils.h"
#include "../include/utils.h"


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void show_usage(void)
{
    fprintf(stderr, "oshell usage: \n"
        "\tthis is just a basic shell, I made for fun.\n"
        "\n\texit: will exit the shell\n"
        "\thelp: will print this menu\n"
    "\n");
}


char* remove_char(char* str, char find) {
    char *src = str, *dst = str;
    while (*src) {
        if (*src != find) {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
    return str;
}

int change_directory(char *directory)
{
    // add /home/user/ if ~ is the first char
    char* new_path = NULL;

    bool use_home_path = false;
    if(directory[0] == '~') {
        char *user_path = getenv("HOME");
        int new_path_length = strlen(user_path) + strlen(directory) + 1;
        // first char is ~ replace dir with /home/user/dir
        remove_char(directory, '~');
        // fprintf(stderr, "%s%s",user_path, directory);
        new_path = malloc(new_path_length);
        if(new_path == NULL) {
            perror("oshell: change_directory() memory allocation failed");
        }
        snprintf(new_path, new_path_length, "%s%s", user_path, directory);
        // fprintf(stderr, "%s\n", new_path);
        use_home_path = true;
    }
    
    int res = 0;
    if(use_home_path) {
        res = chdir(new_path);
    } else {
        res = chdir(directory);
    }

    if (res == -1) {
        free(new_path);
        return -1;
    }
    free(new_path);
    return 0;
}

/* Convert string to int. Returns -1 for invalid input, -2 for overflow */
int string_to_int(char *string) {
    long val = 0;
    int base = 10;
    char *endptr, *str;
    
    str = string;
    errno = 0;    /* Clear errno before strtol call */
    
    val = strtol(str, &endptr, base);
    
    /* Check for numeric overflow/underflow */
    if (errno == ERANGE) {
        perror("strtol");
        return -2;
    }
    
    /* Check for invalid input (no conversion performed) */
    if (endptr == str) { return -1;}
    
    /* TODO: Consider checking for partial conversions (endptr != '\0') */
    /* TODO: Add range validation for int vs long on different platforms */
    
    return (int)val;    /* Cast long to int - potential data loss */
}

// this reads the input
//
// go reads some garbage value as the command and tries to execute that
char *oshell_read_line(void)
{
    char *line = NULL;
    size_t buffsize = 0;

    if(getline(&line, &buffsize, stdin) == -1)
    {
        if(feof(stdin))
        {
            // EOF
            free(line);
            line = NULL;
        } else {
            perror("oshell: readline");
            free(line);
            line = NULL;
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

void oshell_loop(void)
{
    char *tmp_user_input = oshell_read_line();
    char **args = 0;

    char **user_input = malloc(arg_count(tmp_user_input) * sizeof(char*));

    // not sure how but this seems to work
    if(strstr(tmp_user_input, "&&") != NULL) {
        user_input = split_on_chain(tmp_user_input);
    } else {
        user_input[0] = my_strdup(tmp_user_input);
    }
    for(int i = 0; user_input[i] != NULL; i++) {
        args = parse(user_input[i]);
        if(args == NULL) perror("oshell: parsing failed ");
        if (execute_command(args) != 0) break;
    }

    free(user_input);
    free(args);
}

// WARN: everything with make_command has to be freed
char *make_command(char **args) {
    size_t command_len = strlen("/usr/bin/") + strlen(args[0]) + 1;
    // this needs to be freed
    char *command = malloc(command_len);
    if(command == NULL) {
        perror("oshell: not able to allocate enough Memory for command");
        exit(EXIT_FAILURE);
    }

    snprintf(command, command_len ,"/usr/bin/%s", args[0]);
    return command;
}

// claude.ai
int is_numeric(const char *str) {
    if (!str || *str == '\0') return 0;
    while (*str) {
        if (!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}

//One approach is to iterate through the string from the beginning 
//and end, and keep track of the first and last non-whitespace characters. 
//Then, you can create a new string with only the characters between these two positions.

char *strip_non_alpha(char *input_string) {
    for(int i = strlen(input_string) - 1; i >= 0; i--) {
        // fprintf(stderr ,"Character: %c, ASCII: %d\n", input_string[i], input_string[i]);
        // if(input_string[i] == '\n') {
        //     fprintf(stderr, "char is '\\n'");
        // } 
        // if(input_string[i] == '\0') {
        //     fprintf(stderr, "char is '\\0'");
        // }
        if(!isalpha(input_string[i])) {
            // remove i from string
            input_string[i] = '\0';
        }
    }
    return input_string;
}



int find_shell_operator(char* operator, char **args) {
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], operator) == 0) {
            return i;
        }
        // this is to check for $ since it does not stand alone
        if (args[i][0] == operator[0]) {
            return i;
        }
    }
    return -1;
}
/*
 * We want to make a helper function to count the arguments/ strings inside a string 
 * for proper allocation
 * */
int arg_count(char *string) {
    int count = 0; 
    int in_word = 0; // 0 outside, 1 inside

    // we want to loop through the string
    while(string) {
        if(isspace(*string)) {
            // we detect a space meaning we are outside a word/ have a new word
        } else {
            if(in_word == 0) {
                in_word = 1;
                count++;
            }
        }
        string++;
    }
    return count;
}
