#include "syntax.h"

#include <stdlib.h>
#include <stdbool.h>

static TokenList* tokenize_c(const char* source, size_t size) {
    TokenList* list = malloc(sizeof(TokenList));
    list->index = 0;
    list->tokens = Vector_New();
    bool inside_preproc = false;
    bool inside_string = false;

    for (size_t i = 0; i < size; ++i) {
        if (source[i] == '#') {
            inside_preproc = true;
            Token t = { TOKEN_PREPROC, TOKEN_BEGIN, i };
            Vector_Add(list->tokens, Token, t);
        }
        if (inside_preproc && i < size - 1 && source[i + 1] == ' ') {
            inside_preproc = false;
            Token t = { TOKEN_PREPROC, TOKEN_END, i };
            Vector_Add(list->tokens, Token, t);
        }
        if (source[i] == '<') {
            Token t = { TOKEN_STRING, TOKEN_BEGIN, i };
            Vector_Add(list->tokens, Token, t);
        }
        if (source[i] == '>') {
            Token t = { TOKEN_STRING, TOKEN_END, i };
            Vector_Add(list->tokens, Token, t);
        }
        if (source[i] == '"') {
            if (!inside_string) {
                inside_string = true;
                Token t = { TOKEN_STRING, TOKEN_BEGIN, i };
                Vector_Add(list->tokens, Token, t);
            } else {
                inside_string = false;
                Token t = { TOKEN_STRING, TOKEN_END, i };
                Vector_Add(list->tokens, Token, t);
            }
        }
    }

    return list;
}

static TokenList* tokenize_rust(const char* source, size_t size) {
    return NULL;
}

TokenList* tokenize(const char* source, size_t size, SourceType type) {
    switch (type) {
    case SOURCE_TYPE_C:
        return tokenize_c(source, size);
    case SOURCE_TYPE_RUST:
        return tokenize_rust(source, size);
    }
}

void TokenList_Free(TokenList* list)
{
    Vector_Free(list->tokens);
    free(list);
}

Token* TokenList_Current(TokenList* list)
{
    size_t index = list->index;

    if (index < Vector_Len(list->tokens)) {
        return &Vector_At(list->tokens, Token, index);
    }

    return NULL;
}

void TokenList_Advance(TokenList* list)
{
    list->index++;
}
