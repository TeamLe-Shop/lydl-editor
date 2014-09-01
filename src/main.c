#include <stdio.h>
#include <curses.h>
#include "screen.h"

int main(void)
{
    int ch;

    screen_init();

    while (1) {
        erase();
        screen_render();

        ch = getch();
        screen_input(ch);
    }
    return 0;
}
