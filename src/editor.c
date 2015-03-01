#include "editor.h"

#include "buffer.h"

#include <stdlib.h>

struct Editor {
    bool quit_requested;
    BufferList* buffer_list;
    EditorState state;
};


Editor* editor_new()
{
    Editor* editor = malloc(sizeof(Editor));
    editor->quit_requested = false;
    editor->buffer_list = buffer_list_create();
    editor->state = EDITOR_STATE_EDIT;
    return editor;
}


void editor_free(Editor* editor)
{
    buffer_list_free(editor->buffer_list);
    free(editor);
}


bool editor_quit_requested(const Editor* editor)
{
    return editor->quit_requested;
}


Buffer*editor_current_buffer(const Editor* editor)
{
    return editor->buffer_list->list[editor->buffer_list->active];
}


size_t editor_buffer_count(const Editor* editor)
{
    return editor->buffer_list->count;
}


size_t editor_index_of_current_buffer(const Editor* editor)
{
    return editor->buffer_list->active;
}


Buffer*editor_buffer_at(const Editor* editor, size_t index)
{
    return editor->buffer_list->list[index];
}


EditorState editor_state(const Editor* editor)
{
    return editor->state;
}


void editor_switch_to_prev_buffer(Editor* editor)
{
    if (editor->buffer_list->active > 0) {
        editor->buffer_list->active--;
    }
}


void editor_switch_to_next_buffer(Editor* editor)
{
    if (editor->buffer_list->active < editor->buffer_list->count - 1) {
        editor->buffer_list->active++;
    }
}


void editor_request_quit(Editor* editor)
{
    editor->quit_requested = true;
}

void editor_add_buffer(const Editor* editor, Buffer* buffer)
{
    buffer_list_add(editor->buffer_list, buffer);
}

void editor_set_state(Editor* editor, EditorState state)
{
    editor->state = state;
}
