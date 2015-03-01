#include "buffer.h"

#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

size_t const INITIAL_BUFFER_SIZE = 8;

Buffer *Buffer_Create(char* name)
{
    Buffer* buf = malloc(sizeof(Buffer));
    buf->filename = malloc(strlen(name) + 1);
    strncpy(buf->filename, name, strlen(name) + 1);
    buf->data = malloc(INITIAL_BUFFER_SIZE);
    buf->capacity = INITIAL_BUFFER_SIZE;
    memset(buf->data, 0, buf->capacity);
    buf->end_pos_byte = 0;
    buf->cursor_pos_byte= 0;
    buf->end_pos_char = 0;
    buf->cursor_pos_char = 0;
    buf->is_new = false;
    return buf;
}

void Buffer_Free(Buffer* buf)
{
    free(buf->data);
    free(buf->filename);
    free(buf);
}

void Buffer_Expand(Buffer* buf)
{
    size_t new_capacity = buf->capacity * 2;
    buf->data = realloc(buf->data, new_capacity);
    memset(buf->data + buf->capacity, 0, new_capacity - buf->capacity);
    buf->capacity = new_capacity;
}


void Buffer_EraseChar(Buffer *buf, size_t pos)
{
    // TODO: Right now just decreases end_pos, doesn't delete right char
    int len = -1;

    if (buf->end_pos_byte == 0) {
        return;
    }

    buf->modified = true;

    assert(pos <= buf->end_pos_byte);

    // Seek back until we find a valid multibyte char
    for (int i = 0; i < (buf->data - (buf->data - pos)) + 1; i++) {
        size_t max;
        max = buf->end_pos_byte - (pos + i);
        len = mblen(buf->data + (pos - i), max);
        if (len > 0) {
            break;
        }
    }

    assert(len > -1);

    // Move the rest of the buffer onto the erased region
    memmove(buf->data + (buf->cursor_pos_byte - len),
            buf->data + buf->cursor_pos_byte,
            buf->end_pos_byte - buf->cursor_pos_byte);

    buf->end_pos_byte -= len;
    buf->cursor_pos_byte -= len;
    buf->end_pos_char--;
    buf->cursor_pos_char--;
}

void buffer_insert(Buffer* buf, char* src, size_t len, size_t pos) {
    buf->modified = true;
    while (buf->end_pos_byte + len >= buf->capacity) {
        Buffer_Expand(buf);
    }
    // Make space for the insertion by moving the text after the insertion
    memmove(buf->data + pos + len, buf->data + pos, buf->end_pos_byte - pos);
    memcpy(buf->data + pos, src, len);
    buf->end_pos_byte += len;
    buf->cursor_pos_byte += len;
    // TODO: buffer_insert doesn't always insert just one character
    buf->end_pos_char++;
    buf->cursor_pos_char++;
}

void Buffer_InsertChar(Buffer* buf, int ch, size_t pos)
{
    char* mb = malloc(MB_CUR_MAX);

    int len = wctomb(mb, ch);
    assert(len > 0);
    buffer_insert(buf, mb, len, pos);
    free(mb);
}

// Character length of multibyte string
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
        ++len;
    }
    return len + 1;
}

void Buffer_TryLoadFromFile(Buffer* buf, const char* filename)
{
    set_string_buf(&buf->filename, filename);
    FILE* f = fopen(filename, "r");
    // TODO: We probably want to check if the file
    // exists instead of whether the open failed.
    if (!f) {
        buf->is_new = true;
        return;
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    buf->data = realloc(buf->data, len);
    assert(buf->data && "Realloc failed");
    fread(buf->data, 1, len, f);
    buf->end_pos_byte = len;
    buf->cursor_pos_byte = 0;
    buf->cursor_pos_char = 0;
    buf->end_pos_char = mbstrlen(buf->data, len);
    buf->capacity = len;
    buf->modified = false;
    fclose(f);
}

static size_t prev_valid_mbchar_offset(Buffer* buf) {
    // Keep going back until we find a valid multibyte char
    // Return the byte offset of what we travelled
    int offset = -1;
    for (char * c = buf->data + (buf->cursor_pos_byte - 1); c >= buf->data; --c) {
        if (mblen(c, buf->cursor_pos_byte) > 0) {
            return offset;
        }
        offset--;
    }
    assert(false && "No valid multibyte char found!");
}

static size_t next_valid_mbchar_offset(Buffer * buf) {
    return mblen(buf->data + buf->cursor_pos_byte, buf->end_pos_byte);
}

void Buffer_MoveCursorLeft(Buffer* buffer)
{
    if (buffer->cursor_pos_char > 0) {
        buffer->cursor_pos_char--;
        buffer->cursor_pos_byte += prev_valid_mbchar_offset(buffer);
    }
}

void Buffer_MoveCursorRight(Buffer* buffer) {
    if (buffer->cursor_pos_char < buffer->end_pos_char) {
        buffer->cursor_pos_char++;
        buffer->cursor_pos_byte += next_valid_mbchar_offset(buffer);
    }
}

static void buffer_find_prev_char(const Buffer* buffer, wchar_t ch,
                                  size_t* char_pos, size_t* byte_pos) {
    int consumed = 0;
    int i = 0;

    int from = buffer->cursor_pos_byte - 1;

    if (from <= 0) {
        return;
    }

    for (;;) {
        wchar_t extracted_ch;
        int result = mbtowc(&extracted_ch,
                            buffer->data + from - consumed,
                            MB_CUR_MAX);

        // Continue seeking back if invalid multibyte data is found
        if (result < 0) {
            continue;
        }

        assert(result != 0);

        if (extracted_ch == ch) {
            *char_pos = buffer->cursor_pos_char - i;
            *byte_pos = from - consumed;
            return;
        }

        consumed += result;

        if (from - consumed <= 0) {
            return;
        }

        ++i;
    }
}

void Buffer_MoveCursorUp(Buffer* buffer) {
    size_t char_pos = 0, byte_pos = 0;
    buffer_find_prev_char(buffer, '\n', &char_pos, &byte_pos);
    buffer->cursor_pos_byte = byte_pos;
    buffer->cursor_pos_char = char_pos;
}

static void buffer_find_next_char(Buffer* buffer, wchar_t ch,
                                  size_t* char_pos, size_t* byte_pos) {
    int consumed = 0;
    int i = 0;

    for (;;) {
        wchar_t extracted_ch;
        int result = mbtowc(&extracted_ch,
                            buffer->data + buffer->cursor_pos_byte + consumed,
                            MB_CUR_MAX);

        if (extracted_ch == ch) {
            *char_pos = buffer->cursor_pos_char + i + 1;
            *byte_pos = buffer->cursor_pos_byte
                        + consumed
                        + mblen(buffer->data
                                + buffer->cursor_pos_byte + consumed,
                                MB_CUR_MAX);
            return;
        }

        if (result == 0) {
            *char_pos = buffer->cursor_pos_char + i;
            *byte_pos = buffer->cursor_pos_byte + consumed;
            return;
        } else if (result < 0) {
            assert(false && "Invalid multibyte sequence");
        } else {
            consumed += result;
            ++i;
        }
    }
}

void Buffer_MoveCursorDown(Buffer* buffer) {
    size_t char_pos = 0, byte_pos = 0;
    buffer_find_next_char(buffer, '\n', &char_pos, &byte_pos);
    buffer->cursor_pos_byte = byte_pos;
    buffer->cursor_pos_char = char_pos;
}

bool Buffer_IsNew(const Buffer* buffer) {
    return buffer->is_new;
}

int Buffer_Save(Buffer* buffer) {
    FILE* f = fopen(buffer->filename, "w");

    if (!f) {
        return -1;
    }

    size_t result = fwrite(buffer->data, 1, buffer->end_pos_byte, f);

    fclose(f);

    if (result != buffer->end_pos_byte) {
        return -1;
    }

    buffer->is_new = false;
    buffer->modified = false;

    return 0;
}

void Buffer_Reload(Buffer* buffer) {
    Buffer_TryLoadFromFile(buffer, buffer->filename);
}

BufferList* BufferList_Create()
{
    BufferList* list = malloc(sizeof(BufferList));
    list->list = malloc(sizeof(Buffer*) * 1);
    list->count = 0;
    list->active = 0;
    return list;
}

void BufferList_Add(BufferList* list, Buffer* buffer)
{
    list->count++;
    list->list = realloc(list->list, sizeof(Buffer*) * list->count);
    list->list[list->count - 1] = buffer;
}

void BufferList_Free(BufferList* list)
{
    for (size_t i = 0; i < list->count; i++) {
        Buffer_Free(list->list[i]);
    }
    free(list->list);
    free(list);
}

void Buffer_MarkNew(Buffer* buffer)
{
    buffer->is_new = true;
}
