#include "test_vector.h"

#include "vector.h"
#include "mpool.h"

#include <sacc.h>

#include <assert.h>



void test_vector_basics() {
  mpool_t mpool;
  vector_t vector;
  vector_iter_t it;
  int a = 1, b = 2, c = 3;
  
  mpool = mpool_open(256);
  vector = vector_open(mpool, 3);

  vector_head(vector)[0] = &a;
  vector_head(vector)[1] = &b;
  vector_head(vector)[2] = &c;
  assert(vector_head(vector)[3] == NULL);

  it = vector_head(vector);
  assert(it != NULL);
  assert(*it == &a);

  it = vector_next(it);
  assert(it != NULL);
  assert(*it == &b);

  it = vector_next(it);
  assert(it != NULL);
  assert(*it == &c);

  it = vector_next(it);
  assert(*it == NULL);

  assert(vector_size(vector) == 3);

  vector_close(vector, mpool);
  mpool_close(mpool);
}



void test_vector_cpy() {
  mpool_t mpool;
  vector_t vector1, vector2;
  int a = 1, b = 2, c = 3;
  
  mpool = mpool_open(256);
  vector1 = vector_open(mpool, 3);
  vector2 = vector_open(mpool, 3);

  vector_head(vector1)[0] = &a;
  vector_head(vector1)[1] = &b;
  vector_head(vector1)[2] = &c;

  vector_cpy(vector_head(vector2), vector_head(vector1));

  assert(vector_head(vector2)[0] == &a);
  assert(vector_head(vector2)[1] == &b);
  assert(vector_head(vector2)[2] == &c);

  vector_close(vector1, mpool);
  vector_close(vector2, mpool);
  mpool_close(mpool);
}



void test_vector() {
  test_vector_basics();
  test_vector_cpy();
}
