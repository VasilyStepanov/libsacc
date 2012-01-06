#include "test_mpool.h"

#include "test_utils.h"

#include "mpool.h"
#include "mpool_extra.h"

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
  void *p1, *p2, *p3, *p4, *p5, *p6;
  
  mpool = mpool_open(100);
  p1 = mpool_alloc(mpool, 40);
  p2 = mpool_alloc(mpool, 40);
  p3 = mpool_alloc(mpool, 40);
  p4 = mpool_alloc(mpool, 40);
  p5 = mpool_alloc(mpool, 40);
  p6 = mpool_alloc(mpool, 40);

  mpool_free(mpool, p3);
  mpool_free(mpool, p4);

  mpool_stats(mpool, &pages);

  assert(pages == 2);

  mpool_free(mpool, p1);
  mpool_free(mpool, p2);
  mpool_free(mpool, p5);
  mpool_free(mpool, p6);

  mpool_stats(mpool, &pages);

  assert(pages == 1);

  mpool_close(mpool);
}



void test_mpool_realloc() {
  mpool_t mpool;
  void *p1, *p2, *p3, *p4, *p5;

  mpool = mpool_open(10);
  p1 = mpool_realloc(mpool, NULL, 1);
  *(char*)p1 = 'x';
  p2 = mpool_realloc(mpool, p1, 2);
  p3 = mpool_realloc(mpool, p2, 3);
  p4 = mpool_realloc(mpool, p3, 4);
  p5 = mpool_realloc(mpool, p4, 5);

  assert(p1 == p2);
  assert(p2 == p3);
  assert(p3 == p4);
  assert(p4 != p5);
  assert(*(char*)p5 == 'x');

  mpool_close(mpool);
}



void test_mpool_strdup() {
  mpool_t mpool;
  void *s;
  mpool = mpool_open(256);
  s = mpool_strdup(mpool, "foobar");
  assert_equals("foobar", s);

  mpool_close(mpool);
}



void test_mpool() {
  test_mpool_basics();
  test_mpool_freepage();
  test_mpool_realloc();
  test_mpool_strdup();
}
