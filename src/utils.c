#include "../include/Header.h"
#include <stdio.h>

void show_usage(void)
{
    fprintf(stderr, "oshell usage: \n"
        "\tthis is just a basic shell, I made for fun.\n"
        "\n\texit: will exit the shell\n"
        "\thelp: will print this menu\n"
    "\n");
}

int change_directory(const char *directory)
{
    fprintf(stderr, "changing to %s\n", directory);
    if (chdir(directory) == -1) {
        return 1;
    } else {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        fprintf(stderr,"%s", cwd);
    }
    // set errno for error x
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
    if (endptr == str) { return -1;
    }
    
    /* TODO: Consider checking for partial conversions (endptr != '\0') */
    /* TODO: Add range validation for int vs long on different platforms */
    
    return (int)val;    /* Cast long to int - potential data loss */
}

// this reads the input
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
            exit(EXIT_SUCCESS);
        } else {
            perror("oshell: readline");
            free(line);
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

void oshell_loop(void)
{
    char *user_input = oshell_read_line();
    char **args = parse(user_input);

    execute_command(args);
    free(user_input);
    free(args);
}

// WARN: everything with make_command has to be freed
char *make_command(char **args) {
    size_t scmd_len = strlen("/usr/bin/") + strlen(args[0]) + 1;
    // this needs to be freed
    char *scmd = malloc(scmd_len);
    if(scmd == NULL) {
        perror("oshell: not able to allocate enough Memory for scmd");
        exit(EXIT_FAILURE);
    }

    snprintf(scmd, scmd_len ,"/usr/bin/%s", args[0]);
    return scmd;
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


int find_shell_operator (char *operator ,char **args) {
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], operator) == 0) {
            return i;
        }
    }
    return -1;
}
