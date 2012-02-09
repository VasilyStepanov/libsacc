#include "test_vector.h"

#include "src/vector.h"
#include "src/vector_extra.h"
#include "src/mpool.h"

#include <sacc.h>

#include <assert.h>



static void test_vector_basics() {
  SAC_MPool mpool;
  SAC_Vector vector;
  SAC_VectorIter it;
  int a = 1, b = 2, c = 3;
  
  mpool = SAC_mpool_open(256);
  vector = SAC_vector_open(mpool, 3);

  SAC_vector_head(vector)[0] = &a;
  SAC_vector_head(vector)[1] = &b;
  SAC_vector_head(vector)[2] = &c;
  assert(SAC_vector_head(vector)[3] == NULL);

  it = SAC_vector_head(vector);
  assert(it != NULL);
  assert(*it == &a);

  it = SAC_vector_next(it);
  assert(it != NULL);
  assert(*it == &b);

  it = SAC_vector_next(it);
  assert(it != NULL);
  assert(*it == &c);

  it = SAC_vector_next(it);
  assert(*it == NULL);

  assert(SAC_vector_size(vector) == 3);

  SAC_vector_close(vector, mpool);
  SAC_mpool_close(mpool);
}



static void test_vector_cpy() {
  SAC_MPool mpool;
  SAC_Vector vector1, vector2;
  int a = 1, b = 2, c = 3;
  
  mpool = SAC_mpool_open(256);
  vector1 = SAC_vector_open(mpool, 3);
  vector2 = SAC_vector_open(mpool, 3);

  SAC_vector_head(vector1)[0] = &a;
  SAC_vector_head(vector1)[1] = &b;
  SAC_vector_head(vector1)[2] = &c;

  SAC_vector_cpy(SAC_vector_head(vector2), SAC_vector_head(vector1));

  assert(SAC_vector_head(vector2)[0] == &a);
  assert(SAC_vector_head(vector2)[1] == &b);
  assert(SAC_vector_head(vector2)[2] == &c);

  SAC_vector_close(vector1, mpool);
  SAC_vector_close(vector2, mpool);
  SAC_mpool_close(mpool);
}



static void test_vector_from_list() {
  SAC_MPool mpool;
  SAC_List list;
  SAC_Vector vector;
  int a = 1, b = 2, c = 3;
  
  mpool = SAC_mpool_open(256);
  list = SAC_list_open(mpool);

  SAC_list_push_back(list, mpool, &a);
  SAC_list_push_back(list, mpool, &b);
  SAC_list_push_back(list, mpool, &c);

  vector = SAC_vector_from_list(list, mpool);

  assert(SAC_vector_head(vector)[0] == &a);
  assert(SAC_vector_head(vector)[1] == &b);
  assert(SAC_vector_head(vector)[2] == &c);
  assert(SAC_vector_head(vector)[3] == NULL);

  SAC_list_close(list, mpool);
  SAC_vector_close(vector, mpool);
  SAC_mpool_close(mpool);
}



void test_vector() {
  test_vector_basics();
  test_vector_cpy();
  test_vector_from_list();
}
