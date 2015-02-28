#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include "buffer.h"

int is_dir(const char* path);

// Set a heap allocated string buffer to a new string
void set_string_buf(char** buf, const char* src);
// Copy a string into a newly allocated buffer
char* copy_string_into_new_buf(const char* src);
// Append to a heap allocated string buffer
void append_to_string_buf(char** buf, const char* src);

#define UNUSED(var) (void)(var)

#endif // UTIL_H
