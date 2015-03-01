#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
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
void buffer_try_load_from_file(buffer_t* buf, const char* filename);

void buffer_move_cursor_left(buffer_t* buffer);
void buffer_move_cursor_right(buffer_t* buffer);
void buffer_move_cursor_up(buffer_t* buffer);
void buffer_move_cursor_down(buffer_t* buffer);

// Whether the buffer points to a new file (that doesn't exist on disk yet)
bool buffer_is_new(const buffer_t* buffer);

// Save the contents of the buffer into the file named by the buffer's name
//
// On success, returns 0, on error, returns -1
int buffer_save(buffer_t* buffer);

// Reload the buffer from the corresponding file on disk
void buffer_reload(buffer_t* buffer);

// Create a buffer list
buffer_list_t* buffer_list_create();

// Add a buffer to a buffer list
void buffer_list_add(buffer_list_t* buffer_list, buffer_t*);

// Free all buffers in a buffer list and free the list itself.
void buffer_list_free(buffer_list_t* list);

void buffer_mark_new(buffer_t* buffer);

#endif  // BUFFER_H
