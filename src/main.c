#include "editor.h"
#include "ui.h"

#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <locale.h>

int main(int argc, char** argv)
{
    setlocale(LC_CTYPE, "");
    editor_t* editor = editor_new();

    if (argc > 1) {
        buffer_try_load_from_file(editor_current_buffer(editor), argv[1]);
    }

    if (!ui_init()) {
        editor_free(editor);
        return EXIT_FAILURE;
    }

    while (!editor_quit_requested(editor)) {
        erase();
        ui_render(editor);
        wint_t ch;
        get_wch(&ch);
        ui_handle_input(editor, ch);
    }

    ui_destroy();
    editor_free(editor);

    return EXIT_SUCCESS;
}
