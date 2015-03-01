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
Buffer* Buffer_Create(char* name);

// Free a buffer
void Buffer_Free(Buffer* buf);

// Expand the capacity of a buffer
void Buffer_Expand(Buffer* buf);

// Erase character before position `pos`
void Buffer_EraseChar(Buffer* buf, size_t pos);

// Insert character `ch` at position `pos`
void Buffer_InsertChar(Buffer* buf, int ch, size_t pos);

// Load file contents into buffer
void Buffer_TryLoadFromFile(Buffer* buf, const char* filename);

void Buffer_MoveCursorLeft(Buffer* buffer);
void Buffer_MoveCursorRight(Buffer* buffer);
void Buffer_MoveCursorUp(Buffer* buffer);
void Buffer_MoveCursorDown(Buffer* buffer);

// Whether the buffer points to a new file (that doesn't exist on disk yet)
bool Buffer_IsNew(const Buffer* buffer);

// Save the contents of the buffer into the file named by the buffer's name
//
// On success, returns 0, on error, returns -1
int Buffer_Save(Buffer* buffer);

// Reload the buffer from the corresponding file on disk
void Buffer_Reload(Buffer* buffer);

// Create a buffer list
BufferList* BufferList_Create();

// Add a buffer to a buffer list
void BufferList_Add(BufferList* buffer_list, Buffer*);

// Free all buffers in a buffer list and free the list itself.
void BufferList_Free(BufferList* list);

void Buffer_MarkNew(Buffer* buffer);

#endif  // BUFFER_H
