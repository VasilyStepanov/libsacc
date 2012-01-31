#include "media_query.h"



static SAC_MediaQuery* SAC_media_query_alloc(SAC_MPool mpool,
  SAC_MediaQueryType type)
{
  SAC_MediaQuery *query = (SAC_MediaQuery*)SAC_mpool_alloc(
    mpool, sizeof(SAC_MediaQuery)
  );

  if (query == NULL) return NULL;

  query->mediaQueryType = type;
  return query;
}



SAC_MediaQuery* SAC_media_query_type(SAC_MPool mpool, SAC_STRING type) {
  SAC_MediaQuery *query;

  query = SAC_media_query_alloc(mpool, SAC_TYPE_MEDIA_QUERY);
  if (query == NULL) return query;

  query->desc.type = type;

  return query;
}



SAC_MediaQuery* SAC_media_query_feature(SAC_MPool mpool,
  SAC_STRING name, SAC_LexicalUnit *value)
{
  SAC_MediaQuery *query;

  query = SAC_media_query_alloc(mpool, SAC_FEATURE_MEDIA_QUERY);
  if (query == NULL) return query;

  query->desc.feature.name = name;
  query->desc.feature.value = value;

  return query;
}



SAC_MediaQuery* SAC_media_query_and(SAC_MPool mpool,
  SAC_MediaQuery *firstQuery, SAC_MediaQuery *secondQuery)
{
  SAC_MediaQuery *query;

  query = SAC_media_query_alloc(mpool, SAC_AND_MEDIA_QUERY);
  if (query == NULL) return query;

  query->desc.combinator.firstQuery = firstQuery;
  query->desc.combinator.secondQuery = secondQuery;

  return query;
}



SAC_MediaQuery* SAC_media_query_only(SAC_MPool mpool,
  SAC_MediaQuery *subQuery)
{
  SAC_MediaQuery *query;

  query = SAC_media_query_alloc(mpool, SAC_ONLY_MEDIA_QUERY);
  if (query == NULL) return query;

  query->desc.subQuery = subQuery;

  return query;
}



SAC_MediaQuery* SAC_media_query_not(SAC_MPool mpool,
  SAC_MediaQuery *subQuery)
{
  SAC_MediaQuery *query;

  query = SAC_media_query_alloc(mpool, SAC_NOT_MEDIA_QUERY);
  if (query == NULL) return query;

  query->desc.subQuery = subQuery;

  return query;
}
