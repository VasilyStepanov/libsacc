#include "media_query.h"



SAC_MediaQuery* SAC_media_query_alloc(SAC_MPool mpool,
  SAC_MediaQueryType type)
{
  SAC_MediaQuery *query = (SAC_MediaQuery*)SAC_mpool_alloc(
    mpool, sizeof(SAC_MediaQuery)
  );

  if (query == NULL) return NULL;

  query->mediaQueryType = type;
  return query;
}
