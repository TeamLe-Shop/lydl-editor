#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include "buffer.h"

// Fill a row on the screen.
void fill(char ch, int row);

// Fill a column on the screen.
void fill_vert(char ch, int col);

// Draw the contents of a buffer. (Usually editor contents)
void draw_contents(char* buf, int y, int x);

// Print up to `max` characters from `buf` at position (`x`, `y`).
//
// If the string is longer than `max`, the last 3 characters are replaced
// by "..."
//
void print_up_to(char* buf, int y, int x, size_t max);

int is_dir(const char* path);

void draw_content(buffer_t *buf, int y, int x);

// Set a heap allocated string buffer to a new string
void set_string_buf(char** buf, const char* src);
// Copy a string into a newly allocated buffer
char* copy_string_into_new_buf(const char* src);
// Append to a heap allocated string buffer
void append_to_string_buf(char** buf, const char* src);

#define UNUSED(var) (void)(var)

#endif // UTIL_H
