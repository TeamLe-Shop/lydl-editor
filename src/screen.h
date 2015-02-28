#ifndef SCREEN_H
#define SCREEN_H

#include "editor.h"

#define BLACK_WHITE COLOR_PAIR(1)
#define FILE_COLOR COLOR_PAIR(2)
#define DIR_COLOR COLOR_PAIR(3)
#define SELECTED_COLOR COLOR_PAIR(4)

// Initialize curses
bool screen_init();

// Draw the content.
void screen_render(const editor_t* editor);

// Initialize the color values.
void init_colors(void);

// Input.
void screen_handle_input(editor_t* editor, int ch);

// Destroy curses window
void screen_destroy();

#endif // SCREEN_H
