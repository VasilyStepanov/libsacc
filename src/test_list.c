#include "test_list.h"

#include "list.h"
#include "mpool.h"

#include <sacc.h>

#include <assert.h>



static void test_list_basics() {
  mpool_t mpool;
  list_t list;
  list_iter_t it;
  int a = 1, b = 2, c = 3;
  
  mpool = mpool_open(256);
  list = list_open(mpool);

  assert(list_head(list) == NULL);

  list_push_back(list, mpool, &a);
  list_push_back(list, mpool, &b);
  list_push_back(list, mpool, &c);

  it = list_head(list);
  assert(it != NULL);
  assert(*it == &a);

  it = list_next(it);
  assert(it != NULL);
  assert(*it == &b);

  it = list_next(it);
  assert(it != NULL);
  assert(*it == &c);

  it = list_next(it);
  assert(it == NULL);

  list_close(list, mpool);
  mpool_close(mpool);
}



static void test_list_push_front() {
  mpool_t mpool;
  list_t list;
  list_iter_t it;
  int a = 1, b = 2, c = 3;
  
  mpool = mpool_open(256);
  list = list_open(mpool);

  assert(list_head(list) == NULL);

  list_push_front(list, mpool, &a);
  list_push_front(list, mpool, &b);
  list_push_front(list, mpool, &c);

  it = list_head(list);
  assert(it != NULL);
  assert(*it == &c);

  it = list_next(it);
  assert(it != NULL);
  assert(*it == &b);

  it = list_next(it);
  assert(it != NULL);
  assert(*it == &a);

  it = list_next(it);
  assert(it == NULL);

  assert(list_size(list) == 3);

  list_close(list, mpool);
  mpool_close(mpool);
}



void test_list() {
  test_list_basics();
  test_list_push_front();
}
