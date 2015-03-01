#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"

#include <stdbool.h>

typedef struct Editor Editor;
typedef enum EditorState {
    EDITOR_STATE_EDIT,
    EDITOR_STATE_BUFFERS
} EditorState;

Editor* editor_new();
void editor_free(Editor* editor);
bool editor_quit_requested(const Editor* editor);
Buffer* editor_current_buffer(const Editor* editor);
size_t editor_buffer_count(const Editor* editor);
size_t editor_index_of_current_buffer(const Editor* editor);
Buffer* editor_buffer_at(const Editor* editor, size_t index);
EditorState editor_state(const Editor* editor);
void editor_set_state(Editor* editor, EditorState state);
void editor_switch_to_prev_buffer(Editor* editor);
void editor_switch_to_next_buffer(Editor* editor);
void editor_add_buffer(const Editor* editor, Buffer* buffer);
void editor_request_quit(Editor* editor);

#endif // EDITOR_H
