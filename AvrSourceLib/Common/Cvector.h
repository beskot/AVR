#ifndef _CVECTOR_H_
#define _CVECTOR_H_

#define CVECTOR_INIT_CAPACITY 4

#include <stdint.h>
#include <stdlib.h>

typedef struct{
    void** data;
    int size;
    int capacity;
    size_t element_size;
} cvector;

cvector* cvector_init(size_t __dataSize);
 
int cvector_size(cvector* __v);
 
void cvector_push(cvector* __v, void* __data);
 
int cvector_set(cvector* __v, int __index, void* __data);
 
int cvector_delete(cvector* __v, int __index);
 
void* cvector_get(cvector* __v, int __index);
 
void cvector_clear(cvector* __v);

#endif