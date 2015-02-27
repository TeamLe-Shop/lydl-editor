#ifndef SCREEN_H
#define SCREEN_H

#include "editor.h"

#define BLACK_WHITE COLOR_PAIR(1)
#define FILE_COLOR COLOR_PAIR(2)
#define DIR_COLOR COLOR_PAIR(3)
#define SELECTED_COLOR COLOR_PAIR(4)

// Initialize curses
void screen_init(int argc, char** argv, editor_t* editor);

// Draw the content.
void screen_render(const editor_t* editor);

// Initialize the color values.
void init_colors(void);

// Input.
void screen_input(editor_t* editor, int ch);

// Destroy curses window
void screen_destroy(editor_t* editor);

#endif // SCREEN_H
