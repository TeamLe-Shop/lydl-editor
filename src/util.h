#ifndef UTIL_H
#define UTIL_H

/* Fill a row on the screen. */
void fill(char ch, int row);

/* Fill a column on the screen. */
void fill_vert(char ch, int col);

/* Draw the contents of a buffer. (Usually editor contents) */
void draw_contents(char* buf, int y, int x);

/* Print a string at x and y, but have the last character be
 * elipses if it is > the limit.
 */

void print_limitto(char* buf, int y, int x, int limit);

#endif /* UTIL_H */
