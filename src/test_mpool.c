#include "test_mpool.h"

#include "mpool.h"

#include <assert.h>



void test_mpool_basics() {
  size_t pages;
  mpool_t mpool;
  
  mpool = mpool_open(100);
  mpool_alloc(mpool, 40);
  mpool_alloc(mpool, 40);
  mpool_alloc(mpool, 40);
  mpool_alloc(mpool, 40);

  mpool_stats(mpool, &pages);

  mpool_close(mpool);

  assert(pages == 2);
}



void test_mpool_freepage() {
  size_t pages;
  mpool_t mpool;
  void *p3, *p4;
  
  mpool = mpool_open(100);
  mpool_alloc(mpool, 40);
  mpool_alloc(mpool, 40);
  p3 = mpool_alloc(mpool, 40);
  p4 = mpool_alloc(mpool, 40);
  mpool_alloc(mpool, 40);
  mpool_alloc(mpool, 40);

  mpool_free(mpool, p3);
  mpool_free(mpool, p4);

  mpool_stats(mpool, &pages);

  mpool_close(mpool);

  assert(pages == 2);
}



void test_mpool() {
/*
  test_mpool_basics();
*/
  test_mpool_freepage();
}
