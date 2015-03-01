#include "editor.h"
#include "ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

int main(int argc, char** argv)
{
    int arg;

    // We need to set the correct locale
    // for proper wide character support
    setlocale(LC_CTYPE, "");

    if (!ui_init()) {
        return EXIT_FAILURE;
    }

    editor_t* editor = editor_new();

    for (arg = 1; arg < argc; arg++) {
        buffer_t* buffer = buffer_create(argv[arg]);
        editor_add_buffer(editor, buffer);
        buffer_try_load_from_file(buffer, argv[arg]);
    }

    if (argc < 2) {
        buffer_t* start_buffer = buffer_create("Untitled");
        buffer_mark_new(start_buffer);
        editor_add_buffer(editor, start_buffer);
    }

    while (!editor_quit_requested(editor)) {
        ui_render(editor);
        ui_handle_input(editor);
    }

    ui_destroy();
    editor_free(editor);

    return EXIT_SUCCESS;
}
