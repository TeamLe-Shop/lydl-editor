#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    char* name;
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
} buffer_t;

typedef struct {
    buffer_t** list;
    size_t     count;
    size_t     active;
} buffer_list_t;

// Create a new buffer
buffer_t* buffer_create(char* name);

// Free a buffer
void buffer_free(buffer_t* buf);

// Expand the capacity of a buffer
void buffer_expand(buffer_t* buf);

// Erase character before position `pos`
void buffer_erase_char(buffer_t* buf, size_t pos);

// Insert character `ch` at position `pos`
void buffer_insert_char(buffer_t* buf, int ch, size_t pos);

// Load file contents into buffer
void buffer_load_from_file(buffer_t* buf, const char* filename);

void buffer_move_cursor_left(buffer_t* buffer);
void buffer_move_cursor_right(buffer_t* buffer);

// Create a buffer list
buffer_list_t* buffer_list_create();

// Add a buffer to a buffer list
void buffer_list_add(buffer_list_t* buffer_list, buffer_t*);

// Free all buffers in a buffer list and free the list itself.
void buffer_list_free(buffer_list_t* list);

#endif  // BUFFER_H
