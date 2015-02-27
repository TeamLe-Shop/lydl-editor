#include "editor.h"

#include <stdio.h>
#include <curses.h>
#include "screen.h"
#include <locale.h>

int main(int argc, char** argv)
{
    setlocale(LC_CTYPE, "");
    editor_t* editor = editor_new();
    screen_init(argc, argv, editor);

    while (!editor_quit(editor)) {
        erase();
        screen_render(editor);
        wint_t ch;
        get_wch(&ch);
        screen_input(editor, ch);
    }

    screen_destroy(editor);

    return 0;
}
