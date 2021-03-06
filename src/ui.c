#include "ui.h"
#include "util.h"
#include "buffer.h"
#include "syntax.h"

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

static void paint_begin(TokenList* tokens, size_t idx) {
    Token* tok = TokenList_Current(tokens);

    if (tok == NULL || tok->begin_end != TOKEN_BEGIN) {
        return;
    }

    if (idx == tok->pos) {
        switch (tok->type) {
        case TOKEN_COMMENT:
            attron(DIR_COLOR);
            break;
        case TOKEN_PREPROC:
            attron(PREPROC_COLOR);
            break;
        case TOKEN_STRING:
            attron(STRING_COLOR);
            break;
        }
        TokenList_Advance(tokens);
    }
}

static void paint_end(TokenList* tokens, size_t idx) {
    Token* tok = TokenList_Current(tokens);

    if (tok == NULL || tok->begin_end != TOKEN_END) {
        return;
    }

    if (idx == tok->pos) {
        switch (tok->type) {
        case TOKEN_COMMENT:
            attroff(DIR_COLOR);
            break;
        case TOKEN_PREPROC:
            attroff(PREPROC_COLOR);
            break;
        case TOKEN_STRING:
            attroff(STRING_COLOR);
            break;
        }
        TokenList_Advance(tokens);
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
    TokenList* tokens = tokenize(buf->data, buf->end_pos_byte, SOURCE_TYPE_C);

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
            if (tokens != NULL) {
                paint_begin(tokens, i);
            }
            mvprintw(y_pos, x_pos, "%C", wc);
            if (tokens != NULL) {
                paint_end(tokens, i);
            }
            i += len;
        }
        if (buf->cursor_pos_byte == i) {
            cursor_x = x_pos;
            cursor_y = y_pos;
        }
    }

    TokenList_Free(tokens);

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
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_YELLOW, COLOR_BLUE);
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
    Buffer* current_buffer = Editor_CurrentBuffer(editor);
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    // That's one beautiful UI.

    attron(BLACK_WHITE);
      fill(' ', 0);
      mvprintw(0, 5, "Files");
      char open_string[14];
      sprintf(open_string, "Open (%zu)", Editor_BufferCount(editor));
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
     for (size_t i = 0; i < Editor_BufferCount(editor); i++) {
          if (13 + i > max_y - 2) {
              break;
          }

          if (Editor_IndexOfCurrentBuffer(editor) == i) {
              attroff(FILE_COLOR);
              attron(A_BOLD);
              attron(SELECTED_COLOR);
          }
          char * name = buffer_display_name(Editor_BufferAt(editor, i));
          print_up_to(name, 13 + i, 0, 14);
          free(name);
          if (Editor_IndexOfCurrentBuffer(editor) == i) {
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
    Buffer* current_buffer = Editor_CurrentBuffer(editor);
    if (Editor_State(editor) == EDITOR_STATE_EDIT) {
        switch (ch) {
        case CKEY_BACKSPACE:
            Buffer_EraseChar(current_buffer, current_buffer->cursor_pos_byte);
            break;
        case KEY_LEFT:
            Buffer_MoveCursorLeft(current_buffer);
            break;
        case KEY_RIGHT:
            Buffer_MoveCursorRight(current_buffer);
            break;
        case KEY_UP:
            Buffer_MoveCursorUp(current_buffer);
            break;
        case KEY_DOWN:
            Buffer_MoveCursorDown(current_buffer);
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
                Buffer_InsertChar(current_buffer, ' ',
                                   current_buffer->cursor_pos_byte);
            }
            break;
        case KEY_F(5):
            if (Buffer_Save(current_buffer) == -1) {
                assert(false && "Failed to save file!");
            }
            break;
        case KEY_F(6):
            Editor_SetState(editor, EDITOR_STATE_BUFFERS);
            break;
        case KEY_F(7):
            Buffer_Reload(current_buffer);
            break;
        case KEY_F(10):
            Editor_RequestQuit(editor);
            break;
        default:
            Buffer_InsertChar(current_buffer, ch,
                               current_buffer->cursor_pos_byte);
        }

    } else if (Editor_State(editor) == EDITOR_STATE_BUFFERS) {
        switch (ch) {
        case KEY_F(6):
            Editor_SetState(editor, EDITOR_STATE_EDIT);
            break;
        case KEY_UP:
            Editor_SwitchToPrevBuffer(editor);
            break;
        case KEY_DOWN:
            Editor_SwitchToNextBuffer(editor);
            break;
        }
    }
}

void ui_destroy()
{
    endwin();
}
