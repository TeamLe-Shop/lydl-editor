#ifndef SCREEN_H
#define SCREEN_H

#include "editor.h"

#define BLACK_WHITE COLOR_PAIR(1)
#define FILE_COLOR COLOR_PAIR(2)
#define DIR_COLOR COLOR_PAIR(3)
#define SELECTED_COLOR COLOR_PAIR(4)
#define PREPROC_COLOR COLOR_PAIR(5)
#define STRING_COLOR COLOR_PAIR(6)

// Initialize the user interface
bool ui_init();

// Draw the user interface
void ui_render(const Editor* editor);

// Handle user input
void ui_handle_input(Editor* editor);

// Destroy the user interface
void ui_destroy();

#endif // SCREEN_H
