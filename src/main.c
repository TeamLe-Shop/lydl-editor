#include <stdio.h>
#include <curses.h>
#include "screen.h"

int main(int argc, char** argv)
{
    int ch;

    screen_init(argc, argv);

    while (1) {
        erase();
        screen_render();

        ch = getch();
        screen_input(ch);
    }
    return 0;
}
