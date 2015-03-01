#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdbool.h>

typedef struct Buffer {
    char* filename;
    char* data;
    size_t capacity;
    size_t cursor_pos_byte;
    size_t end_pos_byte;
    // Cursor pos counting multibyte characters as one
    size_t cursor_pos_char;
    // End pos counting multibyte characters as one
    size_t end_pos_char;

    // Whether the buffer contents have been
    // modified since it was loaded/saved
    bool modified;
    bool is_new;
} Buffer;

typedef struct BufferList {
    Buffer** list;
    size_t     count;
    size_t     active;
} BufferList;

// Create a new buffer
Buffer* buffer_create(char* name);

// Free a buffer
void buffer_free(Buffer* buf);

// Expand the capacity of a buffer
void buffer_expand(Buffer* buf);

// Erase character before position `pos`
void buffer_erase_char(Buffer* buf, size_t pos);

// Insert character `ch` at position `pos`
void buffer_insert_char(Buffer* buf, int ch, size_t pos);

// Load file contents into buffer
void buffer_try_load_from_file(Buffer* buf, const char* filename);

void buffer_move_cursor_left(Buffer* buffer);
void buffer_move_cursor_right(Buffer* buffer);
void buffer_move_cursor_up(Buffer* buffer);
void buffer_move_cursor_down(Buffer* buffer);

// Whether the buffer points to a new file (that doesn't exist on disk yet)
bool buffer_is_new(const Buffer* buffer);

// Save the contents of the buffer into the file named by the buffer's name
//
// On success, returns 0, on error, returns -1
int buffer_save(Buffer* buffer);

// Reload the buffer from the corresponding file on disk
void buffer_reload(Buffer* buffer);

// Create a buffer list
BufferList* buffer_list_create();

// Add a buffer to a buffer list
void buffer_list_add(BufferList* buffer_list, Buffer*);

// Free all buffers in a buffer list and free the list itself.
void buffer_list_free(BufferList* list);

void buffer_mark_new(Buffer* buffer);

#endif  // BUFFER_H
