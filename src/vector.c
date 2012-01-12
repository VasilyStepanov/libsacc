#include "vector.h"


#define VECTOR_WRAP(vector) \
  ((SAC_Vector)((char*)(vector) + sizeof(struct _SAC_Vector)))

#define VECTOR_UNWRAP(vector) \
  ((struct _SAC_Vector*)((char*)(vector) - sizeof(struct _SAC_Vector)))

#define VECTOR_UNWRAP_SIZE(vector) VECTOR_UNWRAP(vector)->size

struct _SAC_Vector {
  size_t size;
};



SAC_Vector vector_open(mpool_t mpool, size_t size) {
  SAC_Vector ret = VECTOR_WRAP(mpool_alloc(mpool,
    sizeof(struct _SAC_Vector) + sizeof(void*) * (size + 1)));
  ((void**)ret)[size] = NULL;
  VECTOR_UNWRAP_SIZE(ret) = size;
  return ret;
}



SAC_VectorIter vector_head(SAC_Vector vector) {
  return (SAC_VectorIter)vector;
}



SAC_VectorIter vector_next(SAC_VectorIter iter) {
  return iter + 1;
}



size_t vector_size(SAC_Vector vector) {
  return VECTOR_UNWRAP_SIZE(vector);
}



void vector_close(SAC_Vector vector, mpool_t mpool) {
  mpool_free(mpool, VECTOR_UNWRAP(vector));
}



void vector_cpy(SAC_VectorIter dest, SAC_VectorIter src) {
  for (; *src != NULL; ++src, ++dest) *dest = *src;
  *(++dest) = NULL;
}
