#ifndef VECTOR_H
#define VECTOR_H

#include "mpool.h"

#include <sacc.h>



typedef void* SAC_Vector;

typedef void** SAC_VectorIter;



SAC_Vector SAC_vector_open(SAC_MPool mpool, size_t size);

void SAC_vector_close(SAC_Vector vector, SAC_MPool mpool);

size_t SAC_vector_size(SAC_Vector vector);

SAC_VectorIter SAC_vector_head(SAC_Vector vector);

SAC_VectorIter SAC_vector_next(SAC_VectorIter iter);



#endif
