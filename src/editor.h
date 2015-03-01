#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"

#include <stdbool.h>

typedef struct Editor Editor;
typedef enum EditorState {
    EDITOR_STATE_EDIT,
    EDITOR_STATE_BUFFERS
} EditorState;

Editor* Editor_New();
void Editor_Free(Editor* editor);
bool Editor_QuitRequested(const Editor* editor);
Buffer* Editor_CurrentBuffer(const Editor* editor);
size_t Editor_BufferCount(const Editor* editor);
size_t Editor_IndexOfCurrentBuffer(const Editor* editor);
Buffer* Editor_BufferAt(const Editor* editor, size_t index);
EditorState Editor_State(const Editor* editor);
void Editor_SetState(Editor* editor, EditorState state);
void Editor_SwitchToPrevBuffer(Editor* editor);
void Editor_SwitchToNextBuffer(Editor* editor);
void Editor_AddBuffer(const Editor* editor, Buffer* buffer);
void Editor_RequestQuit(Editor* editor);

#endif // EDITOR_H
