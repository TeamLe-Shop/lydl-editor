#include "util.h"

#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "screen.h"
#include <assert.h>

void fill(char ch, int row)
{
    char c[2] = {ch, '\0'};
    int y, x;
    UNUSED(y);
    getmaxyx(stdscr, y, x);

    for (int i = 0; i < x; i++) {
        mvprintw(row, i, c);
    }
}

void fill_vert(char ch, int col)
{
    int y, x;
    UNUSED(y);
    getmaxyx(stdscr, y, x);

    for (int i = 0; i < x; i++) {
        mvprintw(i, col, "%c", ch);
    }
}

void draw_content(buffer_t* buf, int y, int x)
{
    int x_pos = x, y_pos = y;
    int cursor_x = x, cursor_y = y;
    // bool in_quotes = false;

    for (size_t i = 0; i < buf->end_pos_byte;) {

        if (buf->data[i] == '\n') {
            y_pos++;
            x_pos = x;
            // Force the cursor to appear on empty lines
            mvprintw(y_pos, x_pos + 1, "");
            i++;
        } else {
            wchar_t wc;
            int len;
            x_pos++;
            len = mbtowc(&wc, buf->data + i, buf->capacity - i);
            assert(len > 0 && "Invalid multibyte sequence.");
            if (buf->data[i] == '/' && buf->data[i+1] == '*') {
                attron(DIR_COLOR);
            }
            mvprintw(y_pos, x_pos, "%C", wc);
            if (buf->data[i] == '/' && i > 0 && buf->data[i-1] == '*') {
                attroff(DIR_COLOR);
            }
            i += len;
        }
        if (buf->cursor_pos_byte == i) {
            cursor_x = x_pos;
            cursor_y = y_pos;
        }
    }

    move(cursor_y, cursor_x + 1);
}

void print_up_to(char* buf, int y, int x, size_t max)
{
    // If source is larger than max, we only want to copy
    // max - 3 bytes, as the last 3 characters will be `...`
    size_t copy_this_much;
    size_t src_len = strlen(buf);

    if (src_len <= max) {
        copy_this_much = max;
    } else {
        copy_this_much = max - 3;
    }

    // Allocate enough bytes for max + null terminator
    char* print_buf = malloc(max + 1);
    // Make sure print buffer is null terminated
    memset(print_buf, 0, max + 1);

    strncpy(print_buf, buf, copy_this_much);
    mvprintw(y, x, "%s", print_buf);

    if (src_len > max) {
        attron(A_BOLD);
        mvprintw(y, x + copy_this_much, "...");
        attroff(A_BOLD);
    }

    free(print_buf);
}

int is_dir(const char* path)
{
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}

void set_string_buf(char** buf, const char* src)
{
    const int len = strlen(src) + 1;
    *buf = realloc(*buf, len);
    assert(*buf && "Failed to realloc");
    strncpy(*buf, src, len);
}


char* copy_string_into_new_buf(const char* src)
{
    const int len = strlen(src) + 1;
    char* buf = malloc(len);
    assert(buf && "Failed to alloc");
    strncpy(buf, src, len);
    return buf;
}


void append_to_string_buf(char** buf, const char* src)
{
    const int srclen = strlen(src);
    const int buflen = strlen(*buf);
    *buf = realloc(*buf, srclen + buflen + 1);
    strncpy(*buf + buflen, src, srclen);
    // Null terminate
    (*buf)[buflen + srclen] = '\0';
}
