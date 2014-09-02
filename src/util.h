#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

/* Fill a row on the screen. */
void fill(char ch, int row);

/* Fill a column on the screen. */
void fill_vert(char ch, int col);

/* Draw the contents of a buffer. (Usually editor contents) */
void draw_contents(char* buf, int y, int x);

/* Print a string at x and y, but have the last character be
 * elipses if it is > the limit.
 */

void print_limitto(char* buf, int y, int x, size_t limit);

int is_dir(const char* path);

void draw_content(char* buf, int y, int x);

#define UNUSED(var) (void)(var)

#endif /* UTIL_H */
