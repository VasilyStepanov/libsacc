#include "test_media_query.h"

#include "media_query.h"

#include <assert.h>



static void test_media_query_alloc() {
  SAC_MPool mpool;
  SAC_MediaQuery *query;

  mpool = SAC_mpool_open(256);
  query = SAC_media_query_alloc(mpool, SAC_TYPE_MEDIA_QUERY);

  assert(query->mediaQueryType == SAC_TYPE_MEDIA_QUERY);

  SAC_mpool_close(mpool);
}



void test_media_query() {
  test_media_query_alloc();
}
