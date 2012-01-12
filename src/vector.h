#ifndef VECTOR_H
#define VECTOR_H

#include "mpool.h"

#include <sacc.h>



typedef void* SAC_Vector;

typedef void** SAC_VectorIter;



SAC_Vector vector_open(SAC_MPool mpool, size_t size);

void vector_close(SAC_Vector vector, SAC_MPool mpool);

size_t vector_size(SAC_Vector vector);

SAC_VectorIter vector_head(SAC_Vector vector);

SAC_VectorIter vector_next(SAC_VectorIter iter);

void vector_cpy(SAC_VectorIter dest, SAC_VectorIter src);



#endif
