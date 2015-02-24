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
    buf->end_pos_byte = 0;
    buf->cursor_pos_byte= 0;
    buf->end_pos_char = 0;
    buf->cursor_pos_char = 0;
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

    if (buf->end_pos_byte == 0) {
        return;
    }

    buf->modified = true;

    assert(pos <= buf->end_pos_byte);

    /* Seek back until we find a valid multibyte char */
    for (i = 0; i < (buf->data - (buf->data - pos)) + 1; i++) {
        size_t max;
        max = buf->end_pos_byte - (pos + i);
        len = mblen(buf->data + (pos - i), max);
        if (len > 0) {
            break;
        }
    }

    assert(len > -1);

    buf->end_pos_byte -= len;
    buf->cursor_pos_byte -= len;
    buf->end_pos_char--;
    buf->cursor_pos_char--;
}

void buffer_insert(buffer_t* buf, char* src, size_t len, size_t pos) {
    buf->modified = true;
    while (buf->end_pos_byte + len >= buf->capacity) {
        buffer_expand(buf);
    }
    memcpy(buf->data + pos, src, len);
    buf->end_pos_byte += len;
    buf->cursor_pos_byte += len;
    /* TODO: buffer_insert doesn't always insert just one character */
    buf->end_pos_char++;
    buf->cursor_pos_char++;
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

static void set_name_file_new(buffer_t* buf, const char* filename) {
    const char* str = " (NEW)";
    const size_t filename_len = strlen(filename);
    const size_t new_size = filename_len + strlen(str) + 1;
    buf->name = realloc(buf->name, new_size);
    memset(buf->name, 0, new_size);
    strncpy(buf->name, filename, filename_len + 1);
    strncat(buf->name, " (NEW)", new_size);
}

/* Character length of multibyte string */
static size_t mbstrlen(const char* str, size_t max) {
    size_t len = 0;
    int pos = 0;
    for (;;) {
        pos += mblen(str + pos, max);
        if (pos < 1) {
            break;
        }
        assert(pos > 0);
        if ((size_t)pos >= max) {
            break;
        }
        printf("%d\n", pos);
        ++len;
    }
    return len + 1;
}

void buffer_load_from_file(buffer_t* buf, const char* filename)
{
    FILE* f;
    long len;

    f = fopen(filename, "r");
    if (!f) {
        set_name_file_new(buf, filename);
        return;
    }
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fseek(f, 0, SEEK_SET);
    buf->data = realloc(buf->data, len);
    fread(buf->data, 1, len, f);
    buf->end_pos_byte = len;
    buf->cursor_pos_byte = len;
    buf->end_pos_char = mbstrlen(buf->data, len);
    buf->cursor_pos_char = buf->end_pos_char;
    buf->capacity = len;
    buf->name = realloc(buf->name, strlen(filename) + 1);
    strncpy(buf->name, filename, strlen(filename) + 1);
    fclose(f);
}

buffer_list_t* buffer_list_create()
{
    buffer_list_t* list = malloc(sizeof(buffer_list_t));
    list->list = malloc(sizeof(buffer_t*) * 1);
    list->count = 0;
    list->active = 0;
    return list;
}

void buffer_list_add(buffer_list_t* list, buffer_t* buffer)
{
    list->count++;
    list->list = realloc(list->list, sizeof(buffer_t*) * list->count);
    list->list[list->count - 1] = buffer;
}

void buffer_list_free(buffer_list_t* list)
{
    for (size_t i = 0; i < list->count; i++) {
        buffer_free(list->list[i]);
    }
    free(list->list);
    free(list);
}
