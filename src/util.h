#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

/* Fill a row on the screen. */
void fill(char ch, int row);

/* Fill a column on the screen. */
void fill_vert(char ch, int col);

/* Draw the contents of a buffer. (Usually editor contents) */
void draw_contents(char* buf, int y, int x);

/* Print up to `max` characters from `buf` at position (`x`, `y`).
 *
 * If the string is longer than `max`, the last 3 characters are replaced
 * by "..."
 */
void print_up_to(char* buf, int y, int x, size_t max);

int is_dir(const char* path);

void draw_content(char* buf, int y, int x);

#define UNUSED(var) (void)(var)

#endif /* UTIL_H */
