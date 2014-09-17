#include <curses.h>

#ifdef _WIN32 || __linux__ || __FreeBSD__ /* Not sure about Windows and BSD...*/
    #define CKEY_BACKSPACE KEY_BACKSPACE
#elif __APPLE__ || __MACH__               /* Both are the same */
    #define CKEY_BACKSPACE 127
#endif