#ifndef SYNTAX_H
#define SYNTAX_H

#include "vector.h"

#include <stddef.h>

// The types of tokens
typedef enum TokenType {
    TOKEN_COMMENT,
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    // Preprocessor-style construct
    TOKEN_PREPROC,
    TOKEN_OPERATOR,
    // String literal
    TOKEN_STRING,
    // Numeric literal
    TOKEN_NUM
} TokenType;

typedef enum TokenBeginEnd {
    TOKEN_BEGIN,
    TOKEN_END
} TokenBeginEnd;

typedef struct Token {
    TokenType type;
    TokenBeginEnd begin_end;
    size_t pos;
} Token;

typedef enum SourceType {
    SOURCE_TYPE_C,
    SOURCE_TYPE_RUST // Dank rust
} SourceType;

typedef struct TokenList {
    Vector* tokens;
    size_t index;
} TokenList;

TokenList* tokenize(const char* source, size_t size, SourceType type);
void TokenList_Free(TokenList* list);
Token* TokenList_Current(TokenList* list);
void TokenList_Advance(TokenList* list);

#endif // SYNTAX_H
