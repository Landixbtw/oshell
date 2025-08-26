#ifndef PARSING_UTILS_H
#define PARSING_UTILS_H

char *build_quote_string(char **arg, int start_arg, int start_pos, int end_arg, int end_pos);
char **remove_quotes(char **arg);

#endif
