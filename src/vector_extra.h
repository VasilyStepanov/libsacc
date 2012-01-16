#ifndef VECTOR_EXTRA_H
#define VECTOR_EXTRA_H

#include "vector.h"
#include "list.h"
#include "mpool.h"



void SAC_vector_cpy(SAC_VectorIter dest, SAC_VectorIter src);

SAC_Vector SAC_vector_from_list(SAC_List list, SAC_MPool mpool);



#endif
