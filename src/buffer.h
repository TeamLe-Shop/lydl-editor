#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

typedef struct {
    char* data;
    size_t capacity;
    size_t cursor_pos;
    size_t end_pos;
} buffer_t;

/* Create a new buffer */
buffer_t* buffer_create();

/* Free a buffer */
void buffer_free(buffer_t* buf);

/* Expand the capacity of a buffer */
void buffer_expand(buffer_t* buf);

/* Erase character before position `pos` */
void buffer_erase(buffer_t* buf, size_t pos);

/* Insert character `ch` at position `pos` */
void buffer_insert_char(buffer_t* buf, char ch, size_t pos);

#endif  /* BUFFER_H */
