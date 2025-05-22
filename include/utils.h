#ifndef UTILS_H_
#define UTILS_H_

void show_usage(void);

void help(void);
int change_directory(const char *directory);
int kill_process(char *process_name_or_id);
void oshell_loop();
void clear(void);
int _pipe(char **args);
char *make_command(char **args);

#endif
