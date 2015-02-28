#include "util.h"

#include "ui.h"

#include <sys/stat.h>

#include <string.h>
#include <stdlib.h>
#include <assert.h>

int is_dir(const char* path)
{
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}

void set_string_buf(char** buf, const char* src)
{
    // Do nothing if trying to set the buffer from itself
    if (*buf == src) {
        return;
    }
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
