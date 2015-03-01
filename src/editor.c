#include "editor.h"

#include "buffer.h"

#include <stdlib.h>

struct Editor {
    bool quit_requested;
    BufferList* buffer_list;
    EditorState state;
};


Editor* Editor_New()
{
    Editor* editor = malloc(sizeof(Editor));
    editor->quit_requested = false;
    editor->buffer_list = BufferList_Create();
    editor->state = EDITOR_STATE_EDIT;
    return editor;
}


void Editor_Free(Editor* editor)
{
    BufferList_Free(editor->buffer_list);
    free(editor);
}


bool Editor_QuitRequested(const Editor* editor)
{
    return editor->quit_requested;
}


Buffer*Editor_CurrentBuffer(const Editor* editor)
{
    return editor->buffer_list->list[editor->buffer_list->active];
}


size_t Editor_BufferCount(const Editor* editor)
{
    return editor->buffer_list->count;
}


size_t Editor_IndexOfCurrentBuffer(const Editor* editor)
{
    return editor->buffer_list->active;
}


Buffer*Editor_BufferAt(const Editor* editor, size_t index)
{
    return editor->buffer_list->list[index];
}


EditorState Editor_State(const Editor* editor)
{
    return editor->state;
}


void Editor_SwitchToPrevBuffer(Editor* editor)
{
    if (editor->buffer_list->active > 0) {
        editor->buffer_list->active--;
    }
}


void Editor_SwitchToNextBuffer(Editor* editor)
{
    if (editor->buffer_list->active < editor->buffer_list->count - 1) {
        editor->buffer_list->active++;
    }
}


void Editor_RequestQuit(Editor* editor)
{
    editor->quit_requested = true;
}

void Editor_AddBuffer(const Editor* editor, Buffer* buffer)
{
    BufferList_Add(editor->buffer_list, buffer);
}

void Editor_SetState(Editor* editor, EditorState state)
{
    editor->state = state;
}
