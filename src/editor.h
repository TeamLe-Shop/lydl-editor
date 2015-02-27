#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"

#include <stdbool.h>

typedef struct editor editor_t;

editor_t* editor_new();
void editor_free(editor_t* editor);
bool editor_quit(const editor_t* editor);
buffer_t* editor_current_buffer(const editor_t* editor);
size_t editor_buffer_count(const editor_t* editor);
size_t editor_index_of_current_buffer(const editor_t* editor);
buffer_t* editor_buffer_at(const editor_t* editor, size_t index);
int editor_state(const editor_t* editor);
void editor_switch_to_prev_buffer(editor_t* editor);
void editor_switch_to_next_buffer(editor_t* editor);

#endif // EDITOR_H
