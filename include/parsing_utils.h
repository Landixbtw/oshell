#ifndef PARSING_UTILS_H
#define PARSING_UTILS_H

#include <stddef.h>

char *build_quote_string(char **arg, int start_arg, int start_pos, int end_arg, int end_pos);
char **remove_quotes(char **arg);
void print_hex_dump(const char* data, size_t length);
char *my_strdup(const char *s);

#endif
