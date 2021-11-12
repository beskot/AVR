#include "Cvector.h"

void static _cvector_resize(cvector* __v, int __newCap);

cvector* cvector_init(size_t __dataSize)
{
    cvector * __v = (cvector *)malloc(sizeof(cvector));
    __v->capacity = CVECTOR_INIT_CAPACITY;
    __v->size = 0;
    __v->element_size = __dataSize;
    __v->data = (void **) malloc(CVECTOR_INIT_CAPACITY * sizeof(void*));

    return __v;
}
 
void cvector_push(cvector* __v, void* __data)
{
    if(__v->size >= __v->capacity)
    {
        _cvector_resize(__v, __v->capacity + __v->capacity / 2);
    }    
    
    // __v->data[__v->size] = (void*) malloc(__v->element_size);
    // memcpy(__v->data[__v->size], __data, __v->element_size);
    __v->data[__v->size] = __data;

    __v->size++;
}
 
int cvector_set(cvector* __v, int __index, void* __data)
{
    if(__index >= 0 && __index < __v->size)
    {
        //memcpy(__v->data[__index], __data, __v->element_size);
        __v->data[__index] = __data;
        return 1;
    }
    return 0;
}
 
int cvector_delete(cvector* __v, int __index)
{
    if(__index < 0 || __index > __v->size - 1 || __v->size <= 0)
    {
        return 0;
    }
    
    free(cvector_get(__v, __index));
    
    int i = 0;
    for(; i < __v->size - 1; i++)
    {
        __v->data[i] = __v->data[i + 1];
    }
    
    _cvector_resize(__v, __v->size - 1);
    __v->size--;

    return 1;
}
 
void* cvector_get(cvector* __v, int __index)
{
    if(__index < 0 || __index > __v->size - 1 || __v->size <= 0)
    {
        return NULL;
    }
    return __v->data[__index];
}
 
void cvector_clear(cvector* __v)
{
    int i = 0;
    for(; i < __v->size; i++)
    {
        free(__v->data[i]);
    }

    __v->size = 0;
    free(__v->data);
}
 
void _cvector_resize(cvector* __v, int __newCap)
{
    __v->capacity = __newCap;
    __v->data = realloc(__v->data, __newCap * sizeof(void *));
}

int cvector_size(cvector* __v)
{ 
    return __v->size; 
}