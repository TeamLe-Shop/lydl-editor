#include "screen.h"
#include "util.h"
#include "buffer.h"
#include "constants.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>
#include <ctype.h>
#include <assert.h>

buffer_list_t* buffer_list;

int editor_state = EDITOR;

void screen_init(int argc, char** argv)
{
    initscr();
    if (!has_colors())
    {
        screen_destroy();
        printf("Your terminal doesn't support color. Sad smiley :(\n");
        exit(1);
    }
    start_color();
    init_colors();

    keypad(stdscr, TRUE);
    buffer_t* start_buffer = buffer_create("untitled (NEW)");
    if (argc > 1) {
        buffer_try_load_from_file(start_buffer, argv[1]);
    }
    buffer_list = buffer_list_create();
    buffer_list_add(buffer_list, start_buffer);
}

void init_colors(void)
{
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_GREEN);
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

void screen_render(void)
{
    unsigned int start_file = 0;
    unsigned int end_file = start_file + 12;
    glob_t files;
    buffer_t* current_buffer = buffer_list->list[buffer_list->active];
    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    // That's one beautiful UI.

    attron(BLACK_WHITE);
      fill(' ', 0);
      mvprintw(0, 5, "Files");
      char open_string[14];
      sprintf(open_string, "Open (%zu)", buffer_list->count);
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
     for (size_t i = 0; i < buffer_list->count; i++) {
          if (buffer_list->active == i) {
              attroff(FILE_COLOR);
              attron(A_BOLD);
              attron(SELECTED_COLOR);
          }
          char * name = buffer_display_name(buffer_list->list[i]);
          print_up_to(name, 13 + i, 0, 14);
          free(name);
          if (buffer_list->active == i) {
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

void screen_input(int ch)
{
    buffer_t* current_buffer = buffer_list->list[buffer_list->active];
    if (editor_state == EDITOR) {
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
        case KEY_F(5):
            if (buffer_save(current_buffer) == -1) {
                assert(false && "Failed to save file!");
            }
            break;
        case KEY_F(7):
            buffer_reload(current_buffer);
            break;
        default:
            buffer_insert_char(current_buffer, ch,
                               current_buffer->cursor_pos_byte);
        }

    } else if (editor_state == BUFFERS) {
        switch (ch) {
        case KEY_UP:
            if (buffer_list->active > 0) {
                buffer_list->active--;
            }
            break;
        case KEY_DOWN:
            if (buffer_list->active > 0) {
                buffer_list->active--;
            }
            break;
        }
    }
}

void screen_destroy(void)
{
    buffer_list_free(buffer_list);
    endwin();
}
