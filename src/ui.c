#include "ui.h"
#include "util.h"
#include "buffer.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>
#include <ctype.h>
#include <assert.h>

#if defined(_WIN32) || defined(__linux__) || defined(__FreeBSD__)
    // Not sure about Windows and BSD...
    #define CKEY_BACKSPACE KEY_BACKSPACE
#elif defined(__APPLE__) || defined(__MACH__) // Both are the same
    #define CKEY_BACKSPACE 127
#endif

// Fill a row on the screen.
static void fill(char ch, int row)
{
    char c[2] = {ch, '\0'};
    int y, x;
    UNUSED(y);
    getmaxyx(stdscr, y, x);

    for (int i = 0; i < x; i++) {
        mvprintw(row, i, c);
    }
}

// Fill a column on the screen.
static void fill_vert(char ch, int col)
{
    int y, x;
    UNUSED(y);
    getmaxyx(stdscr, y, x);

    for (int i = 0; i < x; i++) {
        mvprintw(i, col, "%c", ch);
    }
}

// Draw the contents of a buffer. (Usually editor contents)
static void draw_content(Buffer* buf, int y, int x)
{
    int x_pos = x, y_pos = y;
    int cursor_x = x, cursor_y = y;
    // bool in_quotes = false;

    int max_y, max_x;
    UNUSED(y);
    getmaxyx(stdscr, max_y, max_x);

    for (size_t i = 0; i < buf->end_pos_byte;) {
        if (y_pos > max_y - 2) {
            break;
        }
        if (x_pos > max_x - 1) {
            continue;
        }
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

// Print up to `max` characters from `buf` at position (`x`, `y`).
//
// If the string is longer than `max`, the last 3 characters are replaced
// by "..."
//
static void print_up_to(char* buf, int y, int x, size_t max)
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

static void init_colors(void)
{
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_GREEN);
}

bool ui_init()
{
    initscr();
    if (!has_colors())
    {
        ui_destroy();
        printf("Your terminal doesn't support color. Sad smiley :(\n");
        return false;
    }
    start_color();
    init_colors();

    keypad(stdscr, TRUE);
    return true;
}

static char * buffer_display_name(const Buffer * buf) {
    char * name = copy_string_into_new_buf(buf->filename);

    if (buf->is_new) {
        append_to_string_buf(&name, " (NEW)");
    } else if (buf->modified) {
        append_to_string_buf(&name, " *");
    }

    return name;
}

void ui_render(const Editor* editor)
{
    erase();
    unsigned int start_file = 0;
    unsigned int end_file = start_file + 12;
    glob_t files;
    Buffer* current_buffer = editor_current_buffer(editor);
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    // That's one beautiful UI.

    attron(BLACK_WHITE);
      fill(' ', 0);
      mvprintw(0, 5, "Files");
      char open_string[14];
      sprintf(open_string, "Open (%zu)", editor_buffer_count(editor));
      mvprintw(12, 0, "%-14s", open_string);

      fill_vert(' ', 14);
      char * name = buffer_display_name(current_buffer);
      mvprintw(0, (max_x / 2) + 7, name);
      free(name);
      fill(' ', max_y-1);
      mvprintw(max_y-1, 15, "Cursor Pos: %zu Byte Pos: %zu  Buffer Len: %zu",
               current_buffer->cursor_pos_char,
               current_buffer->cursor_pos_byte,
               current_buffer->end_pos_char);
    attroff(BLACK_WHITE);

    attron(FILE_COLOR);
      switch (glob("*", 0, NULL, &files)) {
      case 0: // Successful
          break;
      case GLOB_NOSPACE:
          printf("Ran out of memory while listing files.\n");
          break;
      case GLOB_ABORTED:
          printf("Error while reading.\n");
          break;
      case GLOB_NOMATCH:
          break;
      default: break;
      }

      if (end_file > files.gl_pathc) end_file = files.gl_pathc;

      for (size_t file = start_file; file < end_file; file++) {
          if (file + 1 > 11) {
              break;
          }

          if (is_dir(files.gl_pathv[file])) {
              attroff(FILE_COLOR);
              attron(DIR_COLOR);
              attron(A_BOLD);
          }
          print_up_to(files.gl_pathv[file], 1 + file, 0, 14);
          if (is_dir(files.gl_pathv[file])) {
              attroff(DIR_COLOR);
              attroff(A_BOLD);
              attron(FILE_COLOR);
          }
      }

      attron(FILE_COLOR);
     for (size_t i = 0; i < editor_buffer_count(editor); i++) {
          if (13 + i > max_y - 2) {
              break;
          }

          if (editor_index_of_current_buffer(editor) == i) {
              attroff(FILE_COLOR);
              attron(A_BOLD);
              attron(SELECTED_COLOR);
          }
          char * name = buffer_display_name(editor_buffer_at(editor, i));
          print_up_to(name, 13 + i, 0, 14);
          free(name);
          if (editor_index_of_current_buffer(editor) == i) {
              attroff(SELECTED_COLOR);
              attroff(A_BOLD);
              attron(FILE_COLOR);
          }
      }


      globfree(&files);

    attroff(FILE_COLOR);

    // Set the cursor pos to inside the editor.
    move(1, 15);
    draw_content(current_buffer, 1, 14);
}

void ui_handle_input(Editor* editor)
{
    wint_t ch;
    get_wch(&ch);
    Buffer* current_buffer = editor_current_buffer(editor);
    if (editor_state(editor) == EDITOR_STATE_EDIT) {
        switch (ch) {
        case CKEY_BACKSPACE:
            buffer_erase_char(current_buffer, current_buffer->cursor_pos_byte);
            break;
        case KEY_LEFT:
            buffer_move_cursor_left(current_buffer);
            break;
        case KEY_RIGHT:
            buffer_move_cursor_right(current_buffer);
            break;
        case KEY_UP:
            buffer_move_cursor_up(current_buffer);
            break;
        case KEY_DOWN:
            buffer_move_cursor_down(current_buffer);
            break;
        case KEY_HOME:
            // buffer_move_cursor_home(current_buffer);
            break;
        case KEY_END:
            // buffer_move_cursor_end(current_buffer);
            break;
        case KEY_PPAGE:
            // buffer_page_up(current_buffer);
            break;
        case KEY_NPAGE:
            // buffer_page_down(current_buffer);
            break;
        case KEY_DC:
            // buffer_delete_next_char(current_buffer);
            break;
        case '\t':
            // TODO: Proper indentation
            // Now we are just inserting 4 spaces unconditionally
            for (int i = 0; i < 4; ++i) {
                buffer_insert_char(current_buffer, ' ',
                                   current_buffer->cursor_pos_byte);
            }
            break;
        case KEY_F(5):
            if (buffer_save(current_buffer) == -1) {
                assert(false && "Failed to save file!");
            }
            break;
        case KEY_F(6):
            editor_set_state(editor, EDITOR_STATE_BUFFERS);
            break;
        case KEY_F(7):
            buffer_reload(current_buffer);
            break;
        case KEY_F(10):
            editor_request_quit(editor);
            break;
        default:
            buffer_insert_char(current_buffer, ch,
                               current_buffer->cursor_pos_byte);
        }

    } else if (editor_state(editor) == EDITOR_STATE_BUFFERS) {
        switch (ch) {
        case KEY_F(6):
            editor_set_state(editor, EDITOR_STATE_EDIT);
            break;
        case KEY_UP:
            editor_switch_to_prev_buffer(editor);
            break;
        case KEY_DOWN:
            editor_switch_to_next_buffer(editor);
            break;
        }
    }
}

void ui_destroy()
{
    endwin();
}
