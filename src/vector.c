#include "vector.h"


#define VECTOR_WRAP(vector) \
  ((vector_t*)((char*)(vector) + sizeof(struct vector_s)))

#define VECTOR_UNWRAP(vector) \
  ((struct vector_s*)((char*)(vector) - sizeof(struct vector_s)))

#define VECTOR_UNWRAP_SIZE(vector) VECTOR_UNWRAP(vector)->size

struct vector_s {
  size_t size;
};



vector_t vector_open(mpool_t mpool, size_t size) {
  vector_t ret = VECTOR_WRAP(mpool_alloc(mpool,
    sizeof(struct vector_s) + sizeof(void*) * (size + 1)));
  ((void**)ret)[size] = NULL;
  VECTOR_UNWRAP_SIZE(ret) = size;
  return ret;
}



vector_iter_t vector_head(vector_t vector) {
  return (vector_iter_t)vector;
}



vector_iter_t vector_next(vector_iter_t iter) {
  return iter + 1;
}



size_t vector_size(vector_t vector) {
  return VECTOR_UNWRAP_SIZE(vector);
}



void vector_close(vector_t vector, mpool_t mpool) {
  mpool_free(mpool, VECTOR_UNWRAP(vector));
}



void vector_cpy(vector_iter_t dest, vector_iter_t src) {
  for (; *src != NULL; ++src, ++dest) *dest = *src;
  *(++dest) = NULL;
}
