#include "vector.h"



vector_t vector_open(mpool_t mpool, size_t size) {
  vector_t ret = mpool_alloc(mpool, sizeof(void*) * (size + 1));
  ((void**)ret)[size] = NULL;
  return ret;
}



vector_iter_t vector_head(vector_t vector) {
  return (vector_iter_t)vector;
}



vector_iter_t vector_next(vector_iter_t iter) {
  return iter + 1;
}



size_t vector_size(vector_t vector) {
  size_t size = 0;
  vector_iter_t it;

  for (it = vector_head(vector); *it != NULL; it = vector_next(it)) ++size;

  return size;
}



void vector_close(vector_t vector, mpool_t mpool) {
  mpool_free(mpool, vector);
}



void vector_cpy(vector_iter_t dest, vector_iter_t src) {
  for (; *src != NULL; src = vector_next(src), dest = vector_next(dest))
    *dest = *src;
}
