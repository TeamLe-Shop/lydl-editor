#include "editor.h"
#include "ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

int main(int argc, char** argv)
{
    // We need to set the correct locale
    // for proper wide character support
    setlocale(LC_CTYPE, "");

    if (!ui_init()) {
        return EXIT_FAILURE;
    }

    editor_t* editor = editor_new();

    if (argc > 1) {
        buffer_try_load_from_file(editor_current_buffer(editor), argv[1]);
    }

    while (!editor_quit_requested(editor)) {
        ui_render(editor);
        ui_handle_input(editor);
    }

    ui_destroy();
    editor_free(editor);

    return EXIT_SUCCESS;
}
