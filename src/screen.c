#include "screen.h"
#include "util.h"
#include "buffer.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>
#include <ctype.h>

buffer_t* current_buffer;

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
    current_buffer = buffer_create();
    if (argc > 1) {
        buffer_load_from_file(current_buffer, argv[1]);
    }
}

void init_colors(void)
{
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
}

void screen_render(void)
{
    int max_x, max_y;
    size_t file;
    glob_t files;
    UNUSED(max_y);
    getmaxyx(stdscr, max_y, max_x);

    /* That's one beautiful UI. */

    attron(BLACK_WHITE);
      fill(' ', 0);
      mvprintw(0, 3, "Files");

      fill_vert(' ', 14);
      mvprintw(0, (max_x / 2) + 7, "untitled");
    attroff(BLACK_WHITE);

    attron(FILE_COLOR);
      switch (glob("*", 0, NULL, &files)) {
      case 0: /* Successful */
          break;
      case GLOB_NOSPACE:
          printf("Ran out of memory while listing files.\n");
          break;
      case GLOB_ABORTED:
          printf("Error while reading.\n");
          break;
      case GLOB_NOMATCH:
          printf("No files found.\n");
          break;
      default: break;
      }

      for (file = 0; file < files.gl_pathc; file++) {
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

      globfree(&files);

    attroff(FILE_COLOR);

    /* Set the cursor pos to inside the editor. */
    move(1, 15);
    draw_content(current_buffer, 1, 14);
}

void screen_input(int ch)
{
    switch (ch) {
    case KEY_BACKSPACE:
        buffer_erase(current_buffer, current_buffer->end_pos);
        break;
    default:
        buffer_insert_char(current_buffer, ch, current_buffer->end_pos);
    }
}

void screen_destroy(void)
{
    buffer_free(current_buffer);
    endwin();
}
