#include "test_mpool.h"

#include "mpool.h"

#include <assert.h>



void test_mpool_basics() {
  size_t pages;
  mpool_t mpool;
  
  mpool = mpool_open(100);
  mpool_alloc(mpool, 45);
  mpool_alloc(mpool, 45);
  mpool_alloc(mpool, 45);

  mpool_stats(mpool, &pages);

  mpool_close(mpool);

  assert(pages == 2);
}



void test_mpool() {
  test_mpool_basics();
}
