#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"

#include <stdbool.h>

typedef struct editor editor_t;
typedef enum {
    EDITOR_STATE_EDIT,
    EDITOR_STATE_BUFFERS
} editor_state_t;

editor_t* editor_new();
void editor_free(editor_t* editor);
bool editor_quit_requested(const editor_t* editor);
buffer_t* editor_current_buffer(const editor_t* editor);
size_t editor_buffer_count(const editor_t* editor);
size_t editor_index_of_current_buffer(const editor_t* editor);
buffer_t* editor_buffer_at(const editor_t* editor, size_t index);
editor_state_t editor_state(const editor_t* editor);
void editor_set_state(editor_t* editor, editor_state_t state);
void editor_switch_to_prev_buffer(editor_t* editor);
void editor_switch_to_next_buffer(editor_t* editor);
void editor_add_buffer(const editor_t* editor, buffer_t* buffer);
void editor_request_quit(editor_t* editor);

#endif // EDITOR_H
