#include "test_mpool.h"

#include "test_utils.h"

#include "mpool.h"
#include "mpool_extra.h"

#include <assert.h>



static void test_mpool_basics() {
  size_t pages;
  SAC_MPool mpool;
  
  mpool = SAC_mpool_open(100);
  SAC_mpool_alloc(mpool, 40);
  SAC_mpool_alloc(mpool, 40);
  SAC_mpool_alloc(mpool, 40);
  SAC_mpool_alloc(mpool, 40);

  SAC_mpool_stats(mpool, &pages);

  SAC_mpool_close(mpool);

  assert(pages == 2);
}



static void test_mpool_freepage() {
  size_t pages;
  SAC_MPool mpool;
  void *p1, *p2, *p3, *p4, *p5, *p6;
  
  mpool = SAC_mpool_open(100);
  p1 = SAC_mpool_alloc(mpool, 40);
  p2 = SAC_mpool_alloc(mpool, 40);
  p3 = SAC_mpool_alloc(mpool, 40);
  p4 = SAC_mpool_alloc(mpool, 40);
  p5 = SAC_mpool_alloc(mpool, 40);
  p6 = SAC_mpool_alloc(mpool, 40);

  SAC_mpool_free(mpool, p3);
  SAC_mpool_free(mpool, p4);

  SAC_mpool_stats(mpool, &pages);

  assert(pages == 2);

  SAC_mpool_free(mpool, p1);
  SAC_mpool_free(mpool, p2);
  SAC_mpool_free(mpool, p5);
  SAC_mpool_free(mpool, p6);

  SAC_mpool_stats(mpool, &pages);

  assert(pages == 1);

  SAC_mpool_close(mpool);
}



static void test_mpool_realloc() {
  SAC_MPool mpool;
  void *p1, *p2, *p3, *p4, *p5;

  mpool = SAC_mpool_open(10);
  p1 = SAC_mpool_realloc(mpool, NULL, 1);
  *(char*)p1 = 'x';
  p2 = SAC_mpool_realloc(mpool, p1, 2);
  p3 = SAC_mpool_realloc(mpool, p2, 3);
  p4 = SAC_mpool_realloc(mpool, p3, 4);
  p5 = SAC_mpool_realloc(mpool, p4, 5);

  assert(p1 == p2);
  assert(p2 == p3);
  assert(p3 == p4);
  assert(p4 != p5);
  assert(*(char*)p5 == 'x');

  SAC_mpool_close(mpool);
}



static void test_mpool_strdup() {
  SAC_MPool mpool;
  char *s;
  mpool = SAC_mpool_open(256);
  s = SAC_mpool_strdup(mpool, "foobar");
  ASSERT_EQUAL_STRINGS("foobar", s);

  SAC_mpool_close(mpool);
}



static void test_mpool_strndup() {
  SAC_MPool mpool;
  char *s;
  mpool = SAC_mpool_open(256);
  s = SAC_mpool_strndup(mpool, "foobar", 3);
  ASSERT_EQUAL_STRINGS("foo", s);

  SAC_mpool_close(mpool);
}



void test_mpool() {
  test_mpool_basics();
  test_mpool_freepage();
  test_mpool_realloc();
  test_mpool_strdup();
  test_mpool_strndup();
}
