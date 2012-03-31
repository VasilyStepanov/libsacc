#include "test_media_query.h"

#include "src/media_query.h"
#include "test_utils.h"

#include <assert.h>



static void test_media_query_factory(void) {
  SAC_MPool mpool;
  SAC_MediaQuery *query;
  SAC_MediaQuery *firstQuery;
  SAC_MediaQuery *secondQuery;
  SAC_MediaQuery *subQuery;

  mpool = SAC_mpool_open(256);

  query = SAC_media_query_type(mpool, "foo");
  assert(query->mediaQueryType == SAC_TYPE_MEDIA_QUERY);
  ASSERT_EQUAL_STRINGS("foo", query->desc.type);

  query = SAC_media_query_feature(mpool, "foo", NULL);
  assert(query->mediaQueryType == SAC_FEATURE_MEDIA_QUERY);
  ASSERT_EQUAL_STRINGS("foo", query->desc.feature.name);
  assert(query->desc.feature.value == NULL);

  firstQuery = SAC_media_query_feature(mpool, "foo", NULL);
  secondQuery = SAC_media_query_feature(mpool, "bar", NULL);
  query = SAC_media_query_and(mpool, firstQuery, secondQuery);
  assert(query->mediaQueryType == SAC_AND_MEDIA_QUERY);
  assert(query->desc.combinator.firstQuery == firstQuery);
  assert(query->desc.combinator.secondQuery == secondQuery);

  subQuery = SAC_media_query_feature(mpool, "foo", NULL);
  query = SAC_media_query_only(mpool, subQuery);
  assert(query->mediaQueryType == SAC_ONLY_MEDIA_QUERY);
  assert(query->desc.subQuery == subQuery);

  subQuery = SAC_media_query_feature(mpool, "foo", NULL);
  query = SAC_media_query_not(mpool, subQuery);
  assert(query->mediaQueryType == SAC_NOT_MEDIA_QUERY);
  assert(query->desc.subQuery == subQuery);

  SAC_mpool_close(mpool);
}



void test_media_query(void) {
  test_media_query_factory();
}
