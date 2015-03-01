#ifndef UTIL_VECTOR_H
#define UTIL_VECTOR_H

#include <stdlib.h>

typedef struct Vector
{
    void* ptr;
    size_t length;
} Vector;

Vector* Vector_New(void);
void Vector_impl_grow(Vector* v, size_t size);
#define Vector_At(vec, type, index) ((type*)vec->ptr)[(index)]
#define Vector_Add(vec, type, item) \
    Vector_impl_grow(vec, sizeof(type)); \
    Vector_At(vec, type, ((vec)->length - 1)) = item;
void Vector_Free(Vector* v);
size_t Vector_Len(Vector* v);

#endif /* UTIL_VECTOR_H */
