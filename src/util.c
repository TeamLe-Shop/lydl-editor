#include "util.h"

#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "screen.h"

void fill(char ch, int row)
{
    int y, x;
    int i;
    char c[2];

    c[0] = ch;
    c[1] = 0;

    getmaxyx(stdscr, y, x);
    for (i = 0; i < x; i++) {
        mvprintw(row, i, c);
    }
}

void fill_vert(char ch, int col)
{
    int y, x;
    int i;

    getmaxyx(stdscr, y, x);
    for (i = 0; i < x; i++) {
        mvprintw(i, col, "%c", ch);
    }
}

void draw_content(char* buf, int y, int x)
{
    int x_pos = x, y_pos = y;
    for (size_t i = 0; i < strlen(buf); i++) {
        if (buf[i] == '\n') {
            y_pos++;
            x_pos = x;
        } else {
            x_pos++;
            mvprintw(y_pos, x_pos, "%c", buf[i]);
        }
    }
}

void print_limitto(char* buf, int y, int x, size_t limit)
{
    char* temp = malloc(strlen(buf));

    strcpy(temp, buf);

    mvprintw(y, x, "%s", temp);
    if (strlen(temp) > limit) {
        attron(A_BOLD);
        mvprintw(y, x + strlen(temp) - 2, "...");
        attroff(A_BOLD);
    }

    free(temp);
}

int is_dir(const char* path)
{
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}
