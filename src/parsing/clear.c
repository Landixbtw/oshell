/*
 * the clear function just "clears" the screen, it does so by moving x amount of pixels down
 * to create the illusion that the screen is cleared
 * */

#include <stdio.h>

#include "../../include/utils.h"

void clear(void)
{
    // TODO: we want to "move" the screen, so far to the top that the screen seems
    // clear
    // I think we can achive this by just printing enough newlines,
    // We just need to figure out how many newlines we need to print
    // to move the screen so far up that it seems clear
    for (int i = 0; i < 50; i++) {
    fprintf(stderr, "\n");
    }
}
