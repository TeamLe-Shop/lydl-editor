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

    Editor* editor = Editor_New();

    for (arg = 1; arg < argc; arg++) {
        Buffer* buffer = Buffer_Create(argv[arg]);
        Editor_AddBuffer(editor, buffer);
        Buffer_TryLoadFromFile(buffer, argv[arg]);
    }

    if (argc < 2) {
        Buffer* start_buffer = Buffer_Create("Untitled");
        Buffer_MarkNew(start_buffer);
        Editor_AddBuffer(editor, start_buffer);
    }

    while (!Editor_QuitRequested(editor)) {
        ui_render(editor);
        ui_handle_input(editor);
    }

    ui_destroy();
    Editor_Free(editor);

    return EXIT_SUCCESS;
}
