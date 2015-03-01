#include "vector.h"

Vector* Vector_New(void)
{
    Vector* v = malloc(sizeof(Vector));
    v->ptr = NULL;
    v->length = 0;
    return v;
}

void Vector_Free(Vector* v)
{
    free(v->ptr);
    free(v);
}

void Vector_impl_grow(Vector* v, size_t size)
{
    ++(v->length);
    v->ptr = realloc(v->ptr, v->length * size);
}

size_t Vector_Len(Vector* v)
{
    return v->length;
}
