/*
 * the clear function just "clears" the screen, it does so by moving x amount of pixels down
 * to create the illusion that the screen is cleared
 * this takes the amount of rows in the terminal and moves the cursor down that amount of rows
 * we just get the amount of rows, and print that amount of newlines
 * https://stackoverflow.com/questions/1022957/getting-terminal-width-in-c
 * */

#include "../include/Header.h"

void clear(void)
{    struct winsize w;
    // stdin , request type, struct for the size
    ioctl(0, TIOCGWINSZ, &w);

    for (int i = 0; i < w.ws_row; i++) {
        fprintf(stderr, "\n");
    }

    fprintf(stderr, "\033[0;0H");
    return;
}
