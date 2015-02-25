#include <stdio.h>
#include <curses.h>
#include "screen.h"
#include <locale.h>

int main(int argc, char** argv)
{
    setlocale(LC_CTYPE, "");
    screen_init(argc, argv);

    while (1) {
        erase();
        screen_render();
        wint_t ch;
        get_wch(&ch);
        screen_input(ch);
    }
    return 0;
}
