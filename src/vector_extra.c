#include "vector_extra.h"



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
