#include "test_list.h"

#include "src/list.h"
#include "src/mpool.h"

#include <sacc.h>

#include <assert.h>



static void test_list_basics() {
  SAC_MPool mpool;
  SAC_List list;
  SAC_ListIter it;
  int a = 1, b = 2, c = 3;
  
  mpool = SAC_mpool_open(256);
  list = SAC_list_open(mpool);

  assert(SAC_list_head(list) == NULL);

  SAC_list_push_back(list, mpool, &a);
  SAC_list_push_back(list, mpool, &b);
  SAC_list_push_back(list, mpool, &c);

  it = SAC_list_head(list);
  assert(it != NULL);
  assert(*it == &a);

  it = SAC_list_next(it);
  assert(it != NULL);
  assert(*it == &b);

  it = SAC_list_next(it);
  assert(it != NULL);
  assert(*it == &c);

  it = SAC_list_next(it);
  assert(it == NULL);

  SAC_list_close(list, mpool);
  SAC_mpool_close(mpool);
}



static void test_list_push_front() {
  SAC_MPool mpool;
  SAC_List list;
  SAC_ListIter it;
  int a = 1, b = 2, c = 3;
  
  mpool = SAC_mpool_open(256);
  list = SAC_list_open(mpool);

  assert(SAC_list_head(list) == NULL);

  SAC_list_push_front(list, mpool, &a);
  SAC_list_push_front(list, mpool, &b);
  SAC_list_push_front(list, mpool, &c);

  it = SAC_list_head(list);
  assert(it != NULL);
  assert(*it == &c);

  it = SAC_list_next(it);
  assert(it != NULL);
  assert(*it == &b);

  it = SAC_list_next(it);
  assert(it != NULL);
  assert(*it == &a);

  it = SAC_list_next(it);
  assert(it == NULL);

  assert(SAC_list_size(list) == 3);

  SAC_list_close(list, mpool);
  SAC_mpool_close(mpool);
}



void test_list() {
  test_list_basics();
  test_list_push_front();
}
