#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>

int getenvvar(char *env) {
    fprintf(stderr, "%s", secure_getenv(env));
    return 0;
}
