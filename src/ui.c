#include "ui.h"
#include "util.h"
#include "buffer.h"
#include "constants.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>
#include <ctype.h>
#include <assert.h>

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

static char * buffer_display_name(const buffer_t * buf) {
    char * name = copy_string_into_new_buf(buf->filename);

    if (buf->is_new) {
        append_to_string_buf(&name, " (NEW)");
    } else if (buf->modified) {
        append_to_string_buf(&name, " *");
    }

    return name;
}

void ui_render(const editor_t* editor)
{
    unsigned int start_file = 0;
    unsigned int end_file = start_file + 12;
    glob_t files;
    buffer_t* current_buffer = editor_current_buffer(editor);
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
          if (is_dir(files.gl_pathv[file])) {
              attroff(FILE_COLOR);
              attron(DIR_COLOR);
              attron(A_BOLD);
          }
          print_up_to(files.gl_pathv[file], 1 + file, 0, 13);
          if (is_dir(files.gl_pathv[file])) {
              attroff(DIR_COLOR);
              attroff(A_BOLD);
              attron(FILE_COLOR);
          }
      }

      attron(FILE_COLOR);
     for (size_t i = 0; i < editor_buffer_count(editor); i++) {
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

void ui_handle_input(editor_t* editor, int ch)
{
    buffer_t* current_buffer = editor_current_buffer(editor);
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
