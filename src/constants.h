#include <curses.h>

#if defined(_WIN32) || defined(__linux__) || defined(__FreeBSD__)
    /* Not sure about Windows and BSD...*/
    #define CKEY_BACKSPACE KEY_BACKSPACE
#elif defined(__APPLE__) || defined(__MACH__) /* Both are the same */
    #define CKEY_BACKSPACE 127
#endif


#define EDITOR 0
#define BUFFERS 1
