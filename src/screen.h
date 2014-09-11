#ifndef SCREEN_H
#define SCREEN_H

#define BLACK_WHITE COLOR_PAIR(1)
#define FILE_COLOR COLOR_PAIR(2)
#define DIR_COLOR COLOR_PAIR(3)

/* Initialize curses */
void screen_init(int argc, char** argv);

/* Draw the content. */
void screen_render(void);

/* Initialize the color values. */
void init_colors(void);

/* Input. */
void screen_input(int ch);

/* Destroy curses window */
void screen_destroy(void);

#endif /* SCREEN_H */
