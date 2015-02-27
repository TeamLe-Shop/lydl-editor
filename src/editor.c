#include "editor.h"

#include "buffer.h"
#include "constants.h"

#include <stdlib.h>

struct editor {
    bool quit_requested;
    buffer_list_t* buffer_list;
    int state;
};


editor_t* editor_new()
{
    editor_t* editor = malloc(sizeof(editor_t));
    editor->quit_requested = false;
    editor->buffer_list = buffer_list_create();
    buffer_t* start_buffer = buffer_create("untitled (NEW)");
    buffer_list_add(editor->buffer_list, start_buffer);
    editor->state = EDITOR;
    return editor;
}


void editor_free(editor_t* editor)
{
    buffer_list_free(editor->buffer_list);
    free(editor);
}


bool editor_quit(const editor_t* editor)
{
    return editor->quit_requested;
}


buffer_t*editor_current_buffer(const editor_t* editor)
{
    return editor->buffer_list->list[editor->buffer_list->active];
}


size_t editor_buffer_count(const editor_t* editor)
{
    return editor->buffer_list->count;
}


size_t editor_index_of_current_buffer(const editor_t* editor)
{
    return editor->buffer_list->active;
}


buffer_t*editor_buffer_at(const editor_t* editor, size_t index)
{
    return editor->buffer_list->list[index];
}


int editor_state(const editor_t* editor)
{
    return editor->state;
}


void editor_switch_to_prev_buffer(editor_t* editor)
{
    if (editor->buffer_list->active > 0) {
        editor->buffer_list->active--;
    }
}


void editor_switch_to_next_buffer(editor_t* editor)
{
    if (editor->buffer_list->active > 0) {
        editor->buffer_list->active--;
    }
}


void editor_request_quit(editor_t* editor)
{
    editor->quit_requested = true;
}
