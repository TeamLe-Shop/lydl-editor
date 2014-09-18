#include "buffer.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

size_t const INITIAL_BUFFER_SIZE = 8;

buffer_t *buffer_create(char* name)
{
    buffer_t* buf = malloc(sizeof(buffer_t));
    buf->name = malloc(strlen(name) + 1);
    strncpy(buf->name, name, strlen(name) + 1);
    buf->data = malloc(INITIAL_BUFFER_SIZE);
    buf->capacity = INITIAL_BUFFER_SIZE;
    memset(buf->data, 0, buf->capacity);
    buf->end_pos = 0;
    buf->cursor_pos= 0;
    return buf;
}

void buffer_free(buffer_t* buf)
{
    free(buf->data);
    free(buf->name);
    free(buf);
}

void buffer_expand(buffer_t* buf)
{
    size_t new_capacity = buf->capacity * 2;
    buf->data = realloc(buf->data, new_capacity);
    memset(buf->data + buf->capacity, 0, new_capacity - buf->capacity);
    buf->capacity = new_capacity;
}


void buffer_erase(buffer_t *buf, size_t pos)
{
    /* TODO: Right now just decreases end_pos, doesn't delete right char */
    int len = -1;
    int i;

    if (buf->end_pos == 0) {
        return;
    }

    buf->modified = true;

    assert(pos <= buf->end_pos);

    /* Seek back until we find a valid multibyte char */
    for (i = 0; i < (buf->data - (buf->data - pos)) + 1; i++) {
        size_t max;
        max = buf->end_pos - (pos + i);
        len = mblen(buf->data + (pos - i), max);
        /* Reset shift state or wit (read man mblen) */
        mblen(NULL, 0);
        if (len > 0) {
            break;
        }
    }

    assert(len > -1);

    buf->end_pos -= len;
    buf->cursor_pos -= len;
}

void buffer_insert(buffer_t* buf, char* src, size_t len, size_t pos) {
    buf->modified = true;
    while (buf->end_pos + len >= buf->capacity) {
        buffer_expand(buf);
    }
    memcpy(buf->data + pos, src, len);
    buf->end_pos += len;
    buf->cursor_pos += len;
}

void buffer_insert_char(buffer_t* buf, int ch, size_t pos)
{
    int len;
    char* mb = malloc(MB_CUR_MAX);

    len = wctomb(mb, ch);
    assert(len > 0);
    buffer_insert(buf, mb, len, pos);
    free(mb);
}


void buffer_load_from_file(buffer_t* buf, const char* filename)
{
    FILE* f;
    long len;

    f = fopen(filename, "r");
    if (!f) {
        buf->name = realloc(buf->name, strlen(filename));
        memset(buf->name, 0, strlen(buf->name));
        strncpy(buf->name, filename, strlen(filename));
        strcat(buf->name, " (NEW)");
        return;
    }
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fseek(f, 0, SEEK_SET);
    buf->data = realloc(buf->data, len);
    fread(buf->data, 1, len, f);
    buf->end_pos = len;
    buf->capacity = len;
    buf->name = realloc(buf->name, strlen(filename));
    strncpy(buf->name, filename, strlen(filename));
    fclose(f);
}
