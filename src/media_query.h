#ifndef MEDIA_QUERY_H
#define MEDIA_QUERY_H

#include "mpool.h"

#include <sacc.h>



SAC_MediaQuery* SAC_media_query_type(SAC_MPool mpool, SAC_STRING type);
SAC_MediaQuery* SAC_media_query_feature(SAC_MPool mpool,
  SAC_STRING name, SAC_LexicalUnit *value);
SAC_MediaQuery* SAC_media_query_and(SAC_MPool mpool,
  SAC_MediaQuery *firstQuery, SAC_MediaQuery *secondQuery);
SAC_MediaQuery* SAC_media_query_only(SAC_MPool mpool, SAC_MediaQuery *subQuery);
SAC_MediaQuery* SAC_media_query_not(SAC_MPool mpool, SAC_MediaQuery *subQuery);



#endif
