#include "buffer.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

size_t const INITIAL_BUFFER_SIZE = 8;

buffer_t *buffer_create()
{
    buffer_t* buf = malloc(sizeof(buffer_t));
    buf->data = malloc(INITIAL_BUFFER_SIZE);
    buf->capacity = INITIAL_BUFFER_SIZE;
    memset(buf->data, 0, buf->capacity);
    buf->end_pos = 0;
    return buf;
}

void buffer_free(buffer_t* buf)
{
    free(buf->data);
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
    /* TODO: Right now just ignores `pos`, and just decreases end_pos */
    if (buf->end_pos > 0) {
        buf->end_pos--;
    }
}


void buffer_insert_char(buffer_t *buf, char ch, size_t pos)
{
    assert(pos <= buf->end_pos);
    buf->data[pos] = ch;
    buf->end_pos++;
    if (buf->end_pos >= buf->capacity) {
        buffer_expand(buf);
    }
}
