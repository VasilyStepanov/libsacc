#ifndef ARRAY_H
#define ARRAY_H

#include "mpool.h"

#include <sacc.h>



typedef void* vector_t;

typedef void** vector_iter_t;



vector_t vector_open(mpool_t mpool, size_t size);

void vector_close(vector_t vector, mpool_t mpool);

size_t vector_size(vector_t vector);

vector_iter_t vector_head(vector_t vector);

vector_iter_t vector_next(vector_iter_t iter);

void vector_cpy(vector_iter_t dest, vector_iter_t src);



#endif
