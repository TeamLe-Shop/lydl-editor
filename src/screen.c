#include "screen.h"
#include "util.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>

char* buffer;
size_t buffer_len = 32;
size_t cursor_pos;

void screen_init(void)
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

    buffer = malloc(32);
    memset(buffer, 0, 32);
}

void init_colors(void)
{
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
}

void screen_render(void)
{
    int max_x, max_y;
    UNUSED(max_y);
    glob_t files;
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
      case 0: // Successful
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

      for (size_t file = 0; file < files.gl_pathc; file++) {
          if (is_dir(files.gl_pathv[file])) attron(A_BOLD);
          print_limitto(files.gl_pathv[file], 1 + file, 0, 9);
          attroff(A_BOLD);
      }

      globfree(&files);

    attroff(FILE_COLOR);

    /* Set the cursor pos to inside the editor. */
    move(1, 15);
    draw_content(buffer, 1, 14);
}

void screen_input(int ch)
{
    if (ch == 127) {
        if (strlen(buffer) > 0) {
            buffer[strlen(buffer) - 1] = 0;
            buffer_len--;
        }
    } else {
        if (strlen(buffer) == buffer_len - 1) {
            buffer_len+=8;
            buffer = realloc(buffer, buffer_len);
            memset(buffer + buffer_len - 8, 0, 8);
        }
        buffer[strlen(buffer)] = ch;
    }
}

void screen_destroy(void)
{
    free(buffer);
    endwin();
}
