#include "vector.h"


#define VECTOR_WRAP(vector) \
  ((SAC_Vector)((char*)(vector) + sizeof(struct _SAC_Vector)))

#define VECTOR_UNWRAP(vector) \
  ((struct _SAC_Vector*)((char*)(vector) - sizeof(struct _SAC_Vector)))

#define VECTOR_UNWRAP_SIZE(vector) VECTOR_UNWRAP(vector)->size

struct _SAC_Vector {
  size_t size;
};



SAC_Vector SAC_vector_open(SAC_MPool mpool, size_t size) {
  SAC_Vector ret = VECTOR_WRAP(SAC_mpool_alloc(mpool,
    sizeof(struct _SAC_Vector) + sizeof(void*) * (size + 1)));

  if (ret == NULL) return NULL;

  ((void**)ret)[size] = NULL;
  VECTOR_UNWRAP_SIZE(ret) = size;
  return ret;
}



SAC_VectorIter SAC_vector_head(SAC_Vector vector) {
  return (SAC_VectorIter)vector;
}



SAC_VectorIter SAC_vector_next(SAC_VectorIter iter) {
  return iter + 1;
}



size_t SAC_vector_size(SAC_Vector vector) {
  return VECTOR_UNWRAP_SIZE(vector);
}



void SAC_vector_close(SAC_Vector vector, SAC_MPool mpool) {
  SAC_mpool_free(mpool, VECTOR_UNWRAP(vector));
}



void SAC_vector_cpy(SAC_VectorIter dest, SAC_VectorIter src) {
  for (; *src != NULL; ++src, ++dest) *dest = *src;
  *(++dest) = NULL;
}



SAC_Vector SAC_vector_from_list(SAC_List list, SAC_MPool mpool) {
  SAC_Vector vector;
  SAC_VectorIter vit;
  SAC_ListIter lit;

  vector = SAC_vector_open(mpool, SAC_list_size(list));

  if (vector == NULL) return NULL;

  for (lit = SAC_list_head(list), vit = SAC_vector_head(vector);
       lit != NULL;
       lit = SAC_list_next(lit), ++vit)
  {
    *vit = *lit;
  }

  return vector;
}
