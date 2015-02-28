#ifndef SCREEN_H
#define SCREEN_H

#include "editor.h"

#define BLACK_WHITE COLOR_PAIR(1)
#define FILE_COLOR COLOR_PAIR(2)
#define DIR_COLOR COLOR_PAIR(3)
#define SELECTED_COLOR COLOR_PAIR(4)

// Initialize the user interface
bool ui_init();

// Draw the user interface
void ui_render(const editor_t* editor);

// Handle user input
void ui_handle_input(editor_t* editor, int ch);

// Destroy the user interface
void ui_destroy();

#endif // SCREEN_H
